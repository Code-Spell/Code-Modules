import json
import os
import socket

from enum import Enum
from time import sleep

from utils import Position3D, Position2D, Direction


class GameInstruction(Enum):
    WALK = 1
    JUMP = 2
    ROTATE_LEFT = 3
    ROTATE_RIGHT = 4
    ROTATE_BACK = 5


class GameEvent:

    def __init__(self, instruction: GameInstruction, status: bool, message: str, position: Position3D):
        self.instruction = instruction
        self.status = status
        self.message = message
        self.position = position

    def __repr__(self):
        return f'{self.instruction.name}:{self.status.__str__().lower()}:{self.message}:{self.position.x},{self.position.y},{self.position.z}'

    def __str__(self):
        return f'GameEvent(instruction={self.instruction.name}, status={self.status}, message={self.message}, position={self.position})'


class GameRendererConnector:

    def __init__(self):
        self.socket = socket.socket()
        self.game_renderer_host = os.getenv('GAME_RENDERER_HOST', 'localhost')
        self.game_renderer_port = 7777
        self.__connect_to_renderer()

    def __connect_to_renderer(self):
        retries = 0
        while retries < 10:
            try:
                self.socket.connect((self.game_renderer_host, self.game_renderer_port))
                return
            except ConnectionRefusedError:
                retries += 1
                sleep(0.1)
        raise ConnectionRefusedError("Could not connect to game renderer")

    def send_instruction(self, instruction: GameInstruction) -> GameEvent:
        self.socket.send(instruction.name.encode('utf-8'))
        response = self.socket.recv(1024).decode('utf-8')

        response_json = json.loads(response)
        required_fields = ['status', 'message', 'position']

        if not all(field in response_json for field in required_fields):
            raise ValueError("Invalid response from game renderer")

        status = response_json['status']
        message = response_json['message']
        position = Position3D(response_json['position']['x'], response_json['position']['y'], response_json['position']['z'])

        return GameEvent(instruction, status, message, position)

    def obtain_world_info_data(self) -> dict:

        # Request world info size to know the size of the buffer to receive
        self.socket.send('world_info_size'.encode('utf-8'))
        world_info_size = int(self.socket.recv(16).decode('utf-8'))

        # Request world info
        self.socket.send('world_info'.encode('utf-8'))
        response = self.socket.recv(world_info_size).decode('utf-8')

        return json.loads(response)

    def obtain_character_status_data(self) -> dict:

        # Request character status size to know the size of the buffer to receive
        self.socket.send('character_status'.encode('utf-8'))
        response = self.socket.recv(256).decode('utf-8')

        return json.loads(response)


class GameCharacter:

    def __init__(self, game_renderer_connector: GameRendererConnector):
        self.game_renderer_connector = game_renderer_connector
        self.__events: list[GameEvent] = []
        self.current_position = Position3D(0, 0, 0)
        self.next_position = Position2D(0, 0)
        self.current_direction = Direction.UNKNOWN

    def walk(self) -> bool:
        game_event = self.game_renderer_connector.send_instruction(GameInstruction.WALK)
        self.__events.append(game_event)
        return game_event.status

    def turn_backward(self) -> bool:
        game_event = self.game_renderer_connector.send_instruction(GameInstruction.ROTATE_BACK)
        self.__events.append(game_event)
        return game_event.status

    def turn_left(self) -> bool:
        game_event = self.game_renderer_connector.send_instruction(GameInstruction.ROTATE_LEFT)
        self.__events.append(game_event)
        return game_event.status

    def turn_right(self) -> bool:
        game_event = self.game_renderer_connector.send_instruction(GameInstruction.ROTATE_RIGHT)
        self.__events.append(game_event)
        return game_event.status

    def jump(self) -> bool:
        game_event = self.game_renderer_connector.send_instruction(GameInstruction.JUMP)
        self.__events.append(game_event)
        return game_event.status

    def update_character_status(self):
        json_data = self.game_renderer_connector.obtain_character_status_data()
        character_data = json_data['character']
        self.current_position = Position3D(character_data['current_position']['x'], character_data['current_position']['y'],
                                           character_data['current_position']['z'])
        self.next_position = Position2D(character_data['next_position']['x'], character_data['next_position']['z'])
        self.current_direction = Direction(character_data['direction'])

    def apply(self):
        with open('/gamification/game_events.txt', 'w') as log_file:
            for instruction in self.__events:
                log_file.write(f'{instruction.__repr__()}\n')


class GameWorld:

    def __init__(self, game_renderer_connector: GameRendererConnector):
        self.game_renderer_connector = game_renderer_connector
        self.height_map: dict[Position2D, float] = {}

    def load_world(self):
        json_data = self.game_renderer_connector.obtain_world_info_data()
        world_data = json_data['world']
        blocks_data = world_data['blocks']

        for block in blocks_data:
            position_x = block[0]
            position_z = block[1]
            height = block[2]
            self.height_map[Position2D(position_x, position_z)] = height

    def get_height_at(self, position: Position2D) -> float:
        return self.height_map.get(position)
