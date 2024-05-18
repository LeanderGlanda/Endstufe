add_executable(endstufe
        endstufe.c
        drivers/pcm1865.c
        drivers/pcm9211.c
        drivers/pcm5122.c
        drivers/tpa3116d2.c
        )

# pull in common dependencies and additional i2c hardware support
target_link_libraries(endstufe pico_stdlib hardware_i2c)

# create map/bin/hex file etc.
pico_add_extra_outputs(endstufe)

# add url via pico_set_program_url
example_auto_set_url(endstufe)