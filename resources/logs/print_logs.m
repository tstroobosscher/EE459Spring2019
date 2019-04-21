#!/usr/bin/octave-cli -q

data = dlmread("logfile1.txt", ",", 0, 0);

throttle_position = transpose(data(:, 4))

#maf_rate = data(:, 5)

#intake_air_temp = data(:, 6)

#timing_advance = data(:, 7)

#speed = data(:, 8)

#rpm = data(:, 9)

#intake_manifold_pressure = data(:, 10)

#long_term_fuel_trim = data(:, 11)

#short_term_fuel_trim = data(:, 12)

#engine_coolant_temp = data(:, 13)

#calculated_engine_load = data(:, 14)

x = 1:1:length(throttle_position)
throttle_position_plot = figure
subplot(2,1,1)
#title("Throttle Position")
#ylabel("Percent")
#xlabel("Index")
plot(x, throttle_position)
#print(throttle_position_plot, "partc.jpg", "-r300")
pause
