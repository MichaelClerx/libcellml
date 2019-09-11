# The contents of this file was generated from version 0.2.0 of libCellML.

from math import *


__version__ = "0.2.0"

STATE_COUNT = 1
VARIABLE_COUNT = 2

VOI_INFO = {"component": "my_component", "name": "time", "units": "second"}

STATE_INFO = [
    {"component": "my_component", "name": "x", "units": "dimensionless"}
]

VARIABLE_INFO = [
    {"component": "my_component", "name": "b", "units": "second"},
    {"component": "my_component", "name": "a", "units": "second"}
]


def create_states_array():
    return [nan]*1


def create_variables_array():
    return [nan]*2


def initialize_constants(states, variables):
    states[0] = 0.0


def compute_computed_constants(variables):
    pass


def compute_rates(voi, states, rates, variables):
    rates[0] = 1.0


def compute_variables(voi, states, rates, variables):
    variables[0] = 2.0*voi
    variables[1] = 3.0*variables[0]
