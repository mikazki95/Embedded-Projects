# Lanzar red
# Solo se configura el driver pero no la direccion de IP
#io-net -d speedo vid=0x8086,did=0x103a -p tcpip -p qnet
#sleep 4 
#ifconfig en0 172.20.154.251 netmask 255.255.0.0
#route add default 172.20.154.101
