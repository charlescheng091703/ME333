# chapter 28 in python

# sudo apt-get install python3-pip
# python3 -m pip install pyserial
# sudo apt-get install python3-matplotlib

import matplotlib.pyplot as plt 
from statistics import mean 
def read_plot_matrix():
    n_str = ser.read_until(b'\n');  # get the number of data points to receive
    n_int = int(n_str) # turn it into an int
    print('Data length = ' + str(n_int))
    ref = []
    data = []
    data_received = 0
    while data_received < n_int:
        dat_str = ser.read_until(b'\n');  # get the data as a string, ints seperated by spaces
        dat_f = list(map(float,dat_str.split())) # now the data is a list
        ref.append(dat_f[0])
        data.append(dat_f[1])
        data_received = data_received + 1
    meanzip = zip(ref,data)
    meanlist = []
    for i,j in meanzip:
        meanlist.append(abs(i-j))
    score = mean(meanlist)
    t = range(len(ref)) # index array
    t = [0.2*i for i in t]
    plt.plot(t,ref,'r-',t,data,'b*-')
    plt.title('Average error = ' + str(score) + ' degrees')
    plt.ylabel('Angle (degrees)')
    plt.xlabel('Index')
    plt.show()

from genref import genRef
import serial
ser = serial.Serial('/dev/tty.usbserial-DM01MHSJ',230400,rtscts=1)
print('Opening port: ')
print(ser.name)

has_quit = False
mode_dict = {1:"IDLE", 2:"PWM", 3:"ITEST", 4:"HOLD", 5:"TRACK"}
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
    # display the menu options; t6his list will grow
    print('\ta: Read current sensor (mA)\n\r') 
    print('\tc: Read encoder (counts)      \td: Read encoder (deg)\n\r') 
    print('\te: Reset encoder              \tf: Set PWM (-100 to 100)\n\r')
    print('\tg: Set current gain           \th: Get current gains\n\r')
    print('\ti: Set position gains         \tj: Get position gains\n\r')
    print('\tk: Test current control       \tm: Load step trajectory\n\r')
    print('\tn: Load cubic trajectory      \to: Execute trajectory\n\r')
    print('\tp: Unpower the motor          \tq: Quit\n\r')
    print('\tr: Get mode                   \tx: Sum')
    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection+'\n'
     
    # send the command to the PIC32
    ser.write(selection_endline.encode()); # .encode() turns the string into a char array
    
    # take the appropriate action
    # there is no switch() in python, using if elif instead
    if (selection == 'a'):
        n_str = ser.read_until(b'\n')
        n_float = float(n_str) # turn it into an int
        print('The motor current is ' + str(n_float) + ' mA.\n') # print it to the screen
    elif (selection == 'c'):
        n_str = ser.read_until(b'\n')
        n_int = int(n_str) # turn it into an int
        print('The motor angle is ' + str(n_int) + ' counts.\n') # print it to the screen
    elif (selection == 'd'):
        n_str = ser.read_until(b'\n')
        n_int = int(n_str) # turn it into an int
        print('The motor angle is ' + str(n_int) + ' degrees.\n') # print it to the screen
    elif (selection == 'e'):
        n_str = ser.read_until(b'\n')
        n_int = int(n_str) # turn it into an int
        print('The motor angle is ' + str(n_int) + ' counts.\n') # print it to the screen
    elif (selection == 'f'):
        n_str = input('What PWM value would you like [-100 to 100]? ') # get the number to send
        n_int = int(n_str) # turn it into an int
        if (n_int >= 0):
            direction = 'counterclockwise'
        else:
            direction = 'clockwise'
        ser.write((str(n_int)+'\n').encode()); # send the number
        n_str = ser.read_until(b'\n');  # get the incremented number back
        n_int = int(n_str) # turn it into an int
        print('PWM has been set to ' + str(n_int) + '% in the ' + direction + ' direction.\n') # print it to the screen
    elif (selection == 'g'):
        n1_str = input('Enter your desired Kp current gain [recommended: 0.1]: ') # get the number to send
        n1_f = float(n1_str) # turn it into an int
        n2_str = input('Enter your desired Ki current gain [recommended: 0.04]: ') # get the number to send
        n2_f = float(n2_str) # turn it into an int
        print('Sending Kp = ' + str(n1_f) + ' and Ki = ' + str(n2_f) + ' to the current controller.\n') # print it to the screen to double check

        ser.write((str(n1_f)+' '+str(n2_f)+'\n').encode()); # send the number
        n_str = ser.read_until(b'\n');  # get the incremented number back
    elif (selection == 'h'):
        n_str = str(ser.read_until(b'\n'))
        split_str = n_str.split(' ')
        print('The current controller is using Kp = ' + split_str[0][2:] + ' and Ki = ' + split_str[1][:-5] + '.\n') # print it to the screen
    elif (selection == 'i'):
        n1_str = input('Enter your desired Kp position gain [recommended: 250.0]: ') # get the number to send
        n1_f = float(n1_str) # turn it into an int
        n2_str = input('Enter your desired Ki position gain [recommended: 0.01]: ') # get the number to send
        n2_f = float(n2_str) # turn it into an int
        n3_str = input('Enter your desired Kd position gain [recommended: 175.0]: ') # get the number to send
        n3_f = float(n3_str) # turn it into an int
        print('Sending Kp = ' + str(n1_f) + ', Ki = ' + str(n2_f) + ', and Kd = ' + str(n3_f) + ' to the position controller.\n') # print it to the screen to double check

        ser.write((str(n1_f)+' '+str(n2_f)+' '+str(n3_f)+'\n').encode()); # send the number
        n_str = ser.read_until(b'\n');  # get the incremented number back
    elif (selection == 'j'):
        n_str = str(ser.read_until(b'\n'))
        split_str = n_str.split(' ')
        print('The position controller is using Kp = ' + split_str[0][2:] + ', Ki = ' + split_str[1] + ', and Kd = ' + split_str[2][:-5] + '.\n') # print it to the screen
    elif (selection == 'k'):
        read_plot_matrix()
    elif (selection == 'm'):
        ref = genRef('step')
        t = range(len(ref))
        plt.plot(t,ref,'r*-')
        plt.ylabel('angle in degrees')
        plt.xlabel('index')
        plt.show()
        ser.write((str(len(ref))+'\n').encode())
        for i in ref:
            ser.write((str(i)+'\n').encode())
    elif (selection == 'n'):
        ref = genRef('cubic')
        t = range(len(ref))
        plt.plot(t,ref,'r*-')
        plt.ylabel('angle in degrees')
        plt.xlabel('index')
        plt.show()
        ser.write((str(len(ref))+'\n').encode())
        for i in ref:
            ser.write((str(i)+'\n').encode())
    elif (selection == 'o'):
        read_plot_matrix()
    elif (selection == 'p'):
        n_str = ser.read_until(b'\n')
        n_int = int(n_str) # turn it into an int
        print('Mode: ' + mode_dict[n_int] + '\n') # print it to the screen
    elif (selection == 'q'):
        print('Exiting client')
        has_quit = True; # exit client
        # be sure to close the port
        ser.close()
    elif (selection == 'r'):
        n_str = ser.read_until(b'\n')
        n_int = int(n_str) # turn it into an int
        #print(str(n_int)+'\n')
        print('Mode: ' + mode_dict[n_int] + '\n') # print it to the screen
    elif (selection == 'x'):
        n1_str = input('Enter 1st number: ') # get the number to send
        n1_int = int(n1_str) # turn it into an int
        n2_str = input('Enter 2nd number: ') # get the number to send
        n2_int = int(n2_str) # turn it into an int
        print('expression = ' + str(n1_int) + ' + ' + str(n2_int)) # print it to the screen to double check

        ser.write((str(n1_int)+' '+str(n2_int)+'\n').encode()); # send the number
        n_str = ser.read_until(b'\n');  # get the incremented number back
        n_int = int(n_str) # turn it into an int
        print('Got back: ' + str(n_int) + '\n') # print it to the screen
    else:
        print('Invalid Selection ' + selection_endline)



