from enum import Enum


class Position3D:

    def __init__(self, x: float, y: float, z: float):
        self.x = x
        self.y = y
        self.z = z

    def __str__(self):
        return f'Position3D(x={self.x}, y={self.y}, z={self.z})'


class Position2D:

    def __init__(self, x: float, z: float):
        self.x = x
        self.z = z

    def __str__(self):
        return f'Position2D(x={self.x}, z={self.z})'


class Direction(Enum):
    X_POSITIVE = 'XPositive'
    X_NEGATIVE = 'XNegative'
    Z_POSITIVE = 'ZPositive'
    Z_NEGATIVE = 'ZNegative'
    UNKNOWN = 'Unknown'
