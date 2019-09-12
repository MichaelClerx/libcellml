# The content of this file was generated using the Python profile of libCellML 0.2.0.

from math import *


LIBCELLML_VERSION = "0.2.0"

STATE_COUNT = 0
VARIABLE_COUNT = 1

VOI_INFO = {"name": "", "units": "", "component": ""}

STATE_INFO = [
]

VARIABLE_INFO = [
    {"name": "x", "units": "dimensionless", "component": "my_component"}
]


def create_states_array():
    return [nan]*0


def create_variables_array():
    return [nan]*1


def initialize_constants(states, variables):
    variables[0] = 1.0


def compute_computed_constants(variables):
    pass


def compute_rates(voi, states, rates, variables):
    pass


def compute_variables(voi, states, rates, variables):
    pass
