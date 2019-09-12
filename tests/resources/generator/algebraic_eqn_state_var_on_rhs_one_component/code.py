# The content of this file was generated using the Python profile of libCellML 0.2.0.

from math import *


LIBCELLML_VERSION = "0.2.0"

STATE_COUNT = 1
VARIABLE_COUNT = 2

VOI_INFO = {"name": "t", "units": "second", "component": "my_model"}

STATE_INFO = [
    {"name": "x", "units": "dimensionless", "component": "my_model"}
]

VARIABLE_INFO = [
    {"name": "a", "units": "per_s", "component": "my_model"},
    {"name": "xx", "units": "dimensionless", "component": "my_model"}
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
