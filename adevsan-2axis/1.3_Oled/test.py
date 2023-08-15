import os

abs_B_angle = 4090
abs_A_angle = 3090

file = open('test.txt', 'w')
file.write(str(abs_B_angle))
file.close()

if ('test.txt' in os.listdir()):
    with open('test.txt') as f:
        lines = f.read()
        f.close()
        abs_A_angle = int(lines)
else:
    abs_A_angle = 0

print(abs_A_angle)
print(type(abs_A_angle))
