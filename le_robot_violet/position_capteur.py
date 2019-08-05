import bluetooth
import sys

#rfcomm connect hci0 98:D3:32:11:68:22 1

# Adresse du module Bluetooth de l'Arduino
bd_addr = "98:D3:32:11:68:22"

port = 1

position = "/tmp/resultat_lecture_robot.txt"


# Cree le socket RFCOMM
sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )

current_data = ""
liste_datas = []

# Ecris dans un fichier
def write_to_file(filename, data):
    global liste_datas
    liste_datas.append(data)
    if len(liste_datas):
        with open(filename, "a") as file:
            for curr_data in liste_datas:
                file.write("%s\n" % (curr_data))
    liste_datas = []

# Affiche l'information recue
def compute_current_data():
    global current_data
    if '@' in current_data:
        split_data = current_data.split('@')
        for line in split_data[:-1]:
            split_info = line.split(' ')
            if split_info[0] == 'P':
                print split_info[1]
                write_to_file(position, split_info[1])
            elif split_info[0] == 'D':
                print ("Moteur droit : %s" %(split_info[1]))
            elif split_info[0] == 'G':
                print("Moteur gauche : %s" % (split_info[1]))
            else:
                print(line)
        current_data = split_data[-1]

# Se connecte au Bluetooth Arduino
try:
    sock.connect((bd_addr, port))
except bluetooth.BluetoothError as e:
    print 'Erreur de connexion au dongle : %s' % (e)
    sys.exit(1)

while True:
    try:
        data = sock.recv(1024)
        if data:
            current_data = current_data + data
            compute_current_data()
    except bluetooth.BluetoothError as e:
        print 'Erreur de lecture du dongle : %s' % (e)

sock.close()
