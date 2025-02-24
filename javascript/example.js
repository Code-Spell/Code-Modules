import {GameCharacter, GameRendererConnector, GameWorld} from "./game.js";

async function main() {

    const gameRendererConnector = await new GameRendererConnector();
    await gameRendererConnector.connectToRenderer();

    const gameCharacter = new GameCharacter(gameRendererConnector);

    let status = await gameCharacter.walk();
    console.log("Walked: " + status);
    status = await gameCharacter.turnRight();
    console.log("Turned right: " + status);
    status = await gameCharacter.turnLeft();
    console.log("Turned left: " + status);
    status = await gameCharacter.turnBackward();
    console.log("Turned backward: " + status);

    await gameCharacter.updateCharacterStatus();

    console.log(gameCharacter.currentPosition);
    console.log(gameCharacter.nextPosition);
    console.log(gameCharacter.currentDirection);

    gameCharacter.apply();

    const gameWorld = new GameWorld(gameRendererConnector);
    await gameWorld.loadWorld();

    console.log(gameWorld.getHeightAt({x: 18, z: -14}));
}

await main();

process.exit(0);