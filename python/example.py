from game import GameRendererConnector, GameCharacter


def main():

    game_renderer_connector = GameRendererConnector()
    game_renderer_connector.connect_to_renderer()

    game_character = GameCharacter(game_renderer_connector)

    game_character.turn_backward()

    success = game_character.walk()
    print(success)

    game_character.update_character_status()
    print(game_character.current_position)
    print(game_character.next_position)

    success = game_character.jump()
    print(success)

    game_character.update_character_status()
    print(game_character.current_position)
    print(game_character.next_position)

    # game_character.apply()


if __name__ == '__main__':
    main()
