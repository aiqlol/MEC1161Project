#include <stdint.h>
#include <stdlib.h>

#include "data.h"

void simulation_data_init(SimulationData *data)
{
    if (data == NULL)
    {
        return;
    }

    data->states = NULL;
    data->count = 0U;
}

int simulation_data_allocate(SimulationData *data, size_t count)
{
    State *new_states;

    if (data == NULL || count == 0U || count > SIZE_MAX / sizeof(State))
    {
        return 0;
    }

    new_states = (State *)calloc(count, sizeof(State));
    if (new_states == NULL)
    {
        return 0;
    }

    /* Replace any previous allocation only after the new allocation succeeds. */
    free(data->states);
    data->states = new_states;
    data->count = count;

    return 1;
}

void simulation_data_free(SimulationData *data)
{
    if (data == NULL)
    {
        return;
    }

    free(data->states);
    data->states = NULL;
    data->count = 0U;
}

int simulation_data_print(const SimulationData *data, FILE *stream)
{
    size_t i;

    if (data == NULL || data->states == NULL || data->count == 0U || stream == NULL)
    {
        return 0;
    }

    if (fprintf(stream, "\n%10s %12s %12s %14s\n",
                "t (s)", "x (m)", "v (m/s)", "a (m/s^2)") < 0)
    {
        return 0;
    }

    for (i = 0U; i < data->count; i++)
    {
        const State *state = &data->states[i];

        if (fprintf(stream, "%10.4f %12.6f %12.6f %14.6f\n",
                    state->t, state->x, state->v, state->a) < 0)
        {
            return 0;
        }
    }

    return ferror(stream) == 0;
}

int simulation_data_export_csv(const SimulationData *data, const char *filename)
{
    FILE *file;
    size_t i;
    int success = 1;

    if (data == NULL || data->states == NULL || data->count == 0U ||
        filename == NULL || filename[0] == '\0')
    {
        return 0;
    }

    file = fopen(filename, "w");
    if (file == NULL)
    {
        return 0;
    }

    /* Keep these column names stable for the Part 4 visualisation code. */
    if (fprintf(file, "time_s,displacement_m,velocity_m_per_s,acceleration_m_per_s2\n") < 0)
    {
        success = 0;
    }

    for (i = 0U; success && i < data->count; i++)
    {
        const State *state = &data->states[i];

        if (fprintf(file, "%.10g,%.10g,%.10g,%.10g\n",
                    state->t, state->x, state->v, state->a) < 0)
        {
            success = 0;
        }
    }

    if (fclose(file) != 0)
    {
        success = 0;
    }

    return success;
}

const State *simulation_data_get(const SimulationData *data, size_t index)
{
    if (data == NULL || data->states == NULL || index >= data->count)
    {
        return NULL;
    }

    return &data->states[index];
}
