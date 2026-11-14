#ifndef DATA_H
#define DATA_H

#include <stddef.h>
#include <stdio.h>

#include "msd.h"

/*
 * Owns the complete set of simulation states.
 *
 * Each element in states is one time-step record containing time,
 * displacement, velocity and acceleration (the State structure in msd.h).
 */
typedef struct
{
    State *states;
    size_t count;
} SimulationData;

/* Initialise an empty data collection before allocating it. */
void simulation_data_init(SimulationData *data);

/*
 * Allocate storage for count simulation states.
 * Returns 1 on success and 0 on invalid arguments or allocation failure.
 * The caller must initialise data before calling this function.
 */
int simulation_data_allocate(SimulationData *data, size_t count);

/* Release allocated memory and reset the collection to an empty state. */
void simulation_data_free(SimulationData *data);

/* Print all stored states as a formatted table to the supplied stream. */
int simulation_data_print(const SimulationData *data, FILE *stream);

/*
 * Export the stored states to a comma-separated values (CSV) file.
 * The first row contains stable column names for future visualisation tools.
 * Returns 1 on success and 0 if the data or file operation is invalid.
 */
int simulation_data_export_csv(const SimulationData *data, const char *filename);

/* Return a read-only pointer to one state, or NULL when index is out of range. */
const State *simulation_data_get(const SimulationData *data, size_t index);

#endif /* DATA_H */
