# The contents of this file was generated from version 0.2.0 of libCellML.

from math import *


__version__ = "0.2.0"

STATE_COUNT = 1
VARIABLE_COUNT = 2

VOI_INFO = {"component": "my_ode", "name": "t", "units": "second"}

STATE_INFO = [
    {"component": "my_ode", "name": "x", "units": "dimensionless"}
]

VARIABLE_INFO = [
    {"component": "my_ode", "name": "a", "units": "per_s"},
    {"component": "my_algebraic_eqn", "name": "xx", "units": "dimensionless"}
]


def create_states_array():
    return [nan]*1


def create_variables_array():
    return [nan]*2


def initialize_constants(states, variables):
    states[0] = 1.0
    variables[0] = 1.0


def compute_computed_constants(variables):
    pass


def compute_rates(voi, states, rates, variables):
    rates[0] = variables[0]


def compute_variables(voi, states, rates, variables):
    variables[1] = 2.0*states[0]
