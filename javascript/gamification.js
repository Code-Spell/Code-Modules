import {GameCharacter, GameRendererConnector, GameWorld} from "./game.js";
import {Direction} from "./utils.js";

const JUMP_HEIGHT = 1.25

export async function moveRobot(onMove) {

    const gameRendererConnector = await new GameRendererConnector();
    await gameRendererConnector.connectToRenderer();

    const gameCharacter = new GameCharacter(gameRendererConnector);
    const gameWorld = new GameWorld(gameRendererConnector);
    await gameWorld.loadWorld();

    let currentPosition, nextPosition;
    let canWalk, canJump, canTurnLeft, canTurnRight;
    let count = 0;
    do {
        await gameCharacter.updateCharacterStatus();
        currentPosition = gameCharacter.currentPosition;
        nextPosition = gameCharacter.nextPosition;
        const height = gameWorld.getHeightAt(nextPosition);
        if (height === 0) {
            canWalk = false;
            canJump = false;
        } else if (height <= (currentPosition.y - 1)) {
            canWalk = true;
            canJump = true;
        } else {
            if (height - (currentPosition.y-1) < JUMP_HEIGHT) {
                canWalk = false;
                canJump = true;
            } else {
                canWalk = false;
                canJump = false;
            }
        }

        const direction = gameCharacter.currentDirection;
        switch (direction) {
            case Direction.X_POSITIVE:
                canTurnLeft = gameWorld.getHeightAt({x: currentPosition.x, z: currentPosition.z - 1}) - (currentPosition.y - 1) < JUMP_HEIGHT;
                canTurnRight = gameWorld.getHeightAt({x: currentPosition.x, z: currentPosition.z + 1}) - (currentPosition.y - 1) < JUMP_HEIGHT;
                break;
            case Direction.X_NEGATIVE:
                canTurnLeft = gameWorld.getHeightAt({x: currentPosition.x, z: currentPosition.z + 1}) - (currentPosition.y-1) < JUMP_HEIGHT;
                canTurnRight = gameWorld.getHeightAt({x: currentPosition.x, z: currentPosition.z - 1}) - (currentPosition.y-1) < JUMP_HEIGHT;
                break;
            case Direction.Z_POSITIVE:
                canTurnLeft = gameWorld.getHeightAt({x: currentPosition.x + 1, z: currentPosition.z}) - (currentPosition.y-1) < JUMP_HEIGHT;
                canTurnRight = gameWorld.getHeightAt({x: currentPosition.x - 1, z: currentPosition.z}) - (currentPosition.y-1) < JUMP_HEIGHT;
                break;
            case Direction.Z_NEGATIVE:
                canTurnLeft = gameWorld.getHeightAt({x: currentPosition.x - 1, z: currentPosition.z}) - (currentPosition.y-1) < JUMP_HEIGHT;
                canTurnRight = gameWorld.getHeightAt({x: currentPosition.x + 1, z: currentPosition.z}) - (currentPosition.y-1) < JUMP_HEIGHT;
                break;
            default:
                canTurnLeft = false;
                canTurnRight = false;
                break;
        }

        const movement = onMove(canWalk, canJump, canTurnLeft, canTurnRight);
        switch (movement) {
            case "WALK":
                await gameCharacter.walk();
                break;
            case "JUMP":
                await gameCharacter.jump();
                break;
            case "ROTATE_LEFT":
                await gameCharacter.turnLeft();
                break;
            case "ROTATE_RIGHT":
                await gameCharacter.turnRight();
                break;
            default:
                break;
        }

        gameCharacter.apply();

    } while (count++ <= 25 && (nextPosition.x !== -10 || nextPosition.z !== 7));

}

