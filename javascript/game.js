import net from 'net';
import fs from 'fs';
import {Direction} from "./utils.js";

export const GameInstruction = Object.freeze({
    WALK: Symbol("WALK"),
    JUMP: Symbol("JUMP"),
    ROTATE_LEFT: Symbol("ROTATE_LEFT"),
    ROTATE_RIGHT: Symbol("ROTATE_RIGHT"),
    ROTATE_BACK: Symbol("ROTATE_BACK"),
})

export class GameEvent {

    #instruction;
    #status;
    #message;
    #position;

    constructor(instruction, status, message, position) {
        this.#instruction = instruction;
        this.#status = status;
        this.#message = message;
        this.#position = position;
        this._instruction = instruction;
        this._message = message;
        this._position = position;
    }

    get status() {
        return this.#status;
    }

    get message() {
        return this._message;
    }

    get position() {
        return this._position;
    }

    serialize() {
        return `${this.#instruction.description}:${this.#status}:${this.#message}:${this.#position.x},${this.#position.y},${this.#position.z}`;
    }

}

export class GameRendererConnector {

    constructor() {
        this.socket = new net.Socket();
        this.gameRendererHost = process.env.GAME_RENDERER_HOST || 'localhost';
        this.gameRendererPort = 7777;
    }

    async connectToRenderer() {
        let retries = 0;

        await new Promise((resolve, reject) => {
            const attemptConnection = () => {
                const onConnect = () => {
                    resolve();
                };

                // Error handling
                const onError = (err) => {
                    if (err.code === 'ECONNREFUSED' && retries < 15) {
                        retries++;
                        // Remove the listener as it will never be called
                        this.socket.removeListener('connect', onConnect);
                        // Retry after a delay
                        setTimeout(attemptConnection, 200);
                    } else {
                        // Clean up listeners
                        this.socket.removeListener('connect', onConnect);
                        this.socket.removeListener('error', onError);
                        reject(new Error("Could not connect to game renderer: " + err.message));
                    }
                };

                // Attempt to connect
                this.socket.connect(this.gameRendererPort, this.gameRendererHost, onConnect);
                // Add error listener
                this.socket.once('error', onError);
            };

            attemptConnection();
        });
    }

    async sendInstruction(instruction) {
        // Send the instruction to the server
        this.socket.write(instruction.description);

        // Parse the response
        let response = await receiveData(this.socket);
        response = JSON.parse(response.toString());

        // Validate the response fields
        const requiredFields = ['status', 'message', 'position'];
        if (!requiredFields.every(field => field in response)) {
            throw new Error("Invalid response from game renderer");
        }

        const {status, message, position} = response;
        return new GameEvent(instruction, status, message, position);
    }

    async obtainWorldInfoData() {
        // Request world size
        this.socket.write('world_info_size');
        // Receive data from the server
        const worldInfoSize = await receiveData(this.socket);

        // Request world data
        this.socket.write('world_info');
        const worldInfoBuffer = await receiveData(this.socket);
        let worldInfo = worldInfoBuffer.toString();

        // Keep receiving data until the full world info is received
        if (worldInfo.length < worldInfoSize) {
            const additionalData = await receiveData(this.socket);
            worldInfo += additionalData.toString();
        }

        return JSON.parse(worldInfo);
    }

    async obtainCharacterStatusData() {
        // Request character status
        this.socket.write('character_status');

        // Parse the response
        const response = await receiveData(this.socket);
        return JSON.parse(response.toString());
    }

}

export class GameCharacter {

    #currentPosition;
    #nextPosition;
    #currentDirection;

    constructor(gameRendererConnector) {
        this.gameRendererConnector = gameRendererConnector;
        this.events = [];
        this.#currentPosition = {x: 0, y: 0, z: 0};
        this.#nextPosition = {x: 0, z: 0};
        this.#currentDirection = Direction.UNKNOWN;
    }

    async walk() {
        const gameEvent = await this.gameRendererConnector.sendInstruction(GameInstruction.WALK);
        this.events.push(gameEvent);
        return gameEvent.status;
    }

    async turnBackward() {
        const gameEvent = await this.gameRendererConnector.sendInstruction(GameInstruction.ROTATE_BACK);
        this.events.push(gameEvent);
        return gameEvent.status;
    }

    async turnLeft() {
        const gameEvent = await this.gameRendererConnector.sendInstruction(GameInstruction.ROTATE_LEFT);
        this.events.push(gameEvent);
        return gameEvent.status;
    }

    async turnRight() {
        const gameEvent = await this.gameRendererConnector.sendInstruction(GameInstruction.ROTATE_RIGHT);
        this.events.push(gameEvent);
        return gameEvent.status;
    }

    async jump() {
        const gameEvent = await this.gameRendererConnector.sendInstruction(GameInstruction.JUMP);
        this.events.push(gameEvent);
        return gameEvent.status;
    }

    async updateCharacterStatus() {
        const jsonData = await this.gameRendererConnector.obtainCharacterStatusData();

        this.#currentPosition = jsonData.character.current_position;
        this.#nextPosition = jsonData.character.next_position;
        this.#currentDirection = jsonData.character.direction;
    }

    apply() {
        const logFilePath = './gamification/game_events.txt';
        fs.writeFileSync(
            logFilePath,
            this.events.map(event => event.serialize()).join('\n'),
            'utf-8'
        );
    }

    get currentPosition() {
        return this.#currentPosition;
    }

    get nextPosition() {
        return this.#nextPosition;
    }

    get currentDirection() {
        return this.#currentDirection;
    }
}

export class GameWorld {

    #gameRendererConnector;
    #heightMap;

    constructor(gameRendererConnector) {
        this.#gameRendererConnector = gameRendererConnector;
        this.#heightMap = new Map(); // Using Map to replicate the behavior of a Python dictionary
    }

    async loadWorld() {
        const jsonData = await this.#gameRendererConnector.obtainWorldInfoData();
        const worldData = jsonData.world;
        const blocksData = worldData.blocks;
        const forestData = worldData.forest;
        const meshesData = worldData.extra_meshes;
        const allData = [...blocksData, ...forestData, ...meshesData];

        for (const object of allData) {
            const [positionX, positionZ, height] = object;
            const positionKey = JSON.stringify({x: positionX, z: positionZ});
            if (this.#heightMap.has(positionKey)) {
                this.#heightMap.set(positionKey, Math.max(this.#heightMap.get(positionKey), height));
            } else {
                this.#heightMap.set(positionKey, height);
            }
        }
    }

    getHeightAt(position) {
        const positionKey = JSON.stringify(position); // Convert position object to string to use as key
        return this.#heightMap.get(positionKey) || null; // Return null if no value is found
    }

}

async function receiveData(socket) {
    return new Promise((resolve, reject) => {
        // Listen for data
        const onData = (data) => {
            socket.removeListener('error', onError);
            resolve(data);
        };

        const onError = (err) => {
            socket.removeListener('data', onData);
            reject(new Error(`Socket error: ${err.message}`));
        };

        // Listen for data
        socket.once('data', onData);

        // Handle errors on the socket
        socket.once('error', onError);

        // Handle timeout if no response is received
        setTimeout(() => {
            socket.removeListener('data', onData);
            socket.removeListener('error', onError);
            reject(new Error("Response timed out"));
        }, 5000);
    });
}