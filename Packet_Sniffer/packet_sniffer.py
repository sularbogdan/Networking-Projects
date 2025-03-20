import socket
import struct
import textwrap
from email.quoprimime import header_length
from importlib.metadata import version

tab = lambda num: "\t" * num
tab(4)

data_tab = lambda num: "\t" * num
data_tab(4)


def main():
    conn = socket.socket(socket.AF_PACKET, socket.SOCK_RAW, socket.ntohs(3))


    while True:
        raw_data, addr = conn.recvfrom(65536)
        dest_mac, src_mac, eth_prot, data = ethernet_frame(raw_data)
        print('\nEthernet Frame:')
        print('Destination: {}, Source: {}, Protocol: {}'.format(dest_mac, src_mac, eth_prot))

        # 8 for IPv4
        if eth_prot == 8:
            (version, header_length, ttl, proto, src, target, data) = ipv4_packet(data)
            print( tab(1) + 'IPv4 Packet: ')
            print(tab(2) + 'Version: {}, Header Length: {}, TTL: {}'.format(version, header_length, ttl))
            print(tab(3) + 'Protocol: {}, Source: {}, Target: {}'.format(proto, src, target))

            #ICMP
            if proto == 1:
                icmp_type, code, checksum, data = icmp_packet(data)
                print(tab(1) + 'ICMP Packet: ')
                print(tab(2) + 'Type: {}, Code: {}, Checksum: {}'.format(icmp_type, code, checksum))
                print(tab(2) + 'Data: ')
                print(format_multi_line(data_tab(3), data))

            # TCP
            elif proto == 6:
                    (src_port, dest_port, sequence, acknowledgement, flag_urg, flag_ack, flag_psh, flag_rst, flag_syn, flag_fin, data) = tcp_segment(data)
                    print(tab(1) + 'TCP Segment: ')
                    print(tab(2) + 'Source Port: {}, Destination Port: {}'.format(src_port, dest_port))
                    print(tab(2) + 'Sequence: {}, Acknowledgment: {}'.format(sequence, acknowledgement))
                    print(tab(2) + 'Flags:')
                    print(tab(3) + 'URG: {}, ACK: {}, PSH: {}, RST: {}, SYN: {} FIN: {}'.format(flag_urg, flag_ack, flag_psh, flag_rst, flag_syn,flag_fin))
                    print(tab(2) + 'Data:')
                    print(format_multi_line(data_tab(3), data))

            #UDP
            elif proto == 17:
               src_port, dest_port, length, data = udp_segment(data)
               print(tab(1) + 'UDP Segment:')
               print(tab(2) + 'Source Port: {}, Destination Port: {}, Length: {}'.format(src_port, dest_port, length))

            #Other
            else:
                print(tab(1) + 'Data:')
                print(format_multi_line(data_tab(2), data))

        else:
            print('Data:')
            print(format_multi_line(data_tab(2), data))


#Unpack ethernet frame
def ethernet_frame(data):
    dest_mac, src_mac, proto = struct.unpack('! 6s 6s H', data[:14])
    return get_mac_addr(dest_mac), get_mac_addr(src_mac), socket.htons(proto), data[14:]

#return properly formatted MAC address (ex AA:BB:CC:DD:EE:FF)
def get_mac_addr(bytes_addr):
    bytes_str = map('{:02x}'.format, bytes_addr)
    mac_addr = ':'.join(bytes_str).upper()
    return mac_addr

#Unpacks Ipv4 packet
def ipv4_packet(data):
    version_header_length = data[0]
    version = version_header_length >> 4
    header_length = (version_header_length & 15) * 4
    ttl, prot, src, target = struct.unpack('! 8x B B 2x 4s 4s', data[:20] )
    return version, header_length, ttl, prot, ipv4(src), ipv4(target), data[header_length:]

#Returns properly formatted IPv4 address
def ipv4(addr):
    return '.'.join(map(str, addr))

# unpacks icmp packet
def icmp_packet(data):
    icmp_type, code, checksum = struct.unpack('! B B H', data[:4])
    return icmp_type,code, checksum, data[4:]

# unpacks tcp packet
def tcp_segment(data):
    (src_port, dest_port, sequence, acknowledgement, offset_reserved_flags) = struct.unpack('! H H L L H', data[:14])
    offset = (offset_reserved_flags & 12) * 4
    flag_urg = (offset_reserved_flags & 32) * 5
    flag_ack = (offset_reserved_flags & 16) * 4
    flag_psh = (offset_reserved_flags & 8) * 3
    flag_rst = (offset_reserved_flags & 4) * 2
    flag_syn = (offset_reserved_flags & 2) * 1
    flag_fin = offset_reserved_flags & 1
    return src_port, dest_port, sequence, acknowledgement, flag_urg, flag_ack, flag_psh, flag_rst, flag_syn,  flag_fin, data[offset:]

# unpacks udp packet
def udp_segment(data):
    src_port, des_port, size = struct.unpack('! H H 2x H', data[:8])
    return src_port, des_port, size, data[8:]

#Formats multi-line data
def format_multi_line(prefix, string, size=80):
    size -= len(prefix)
    if isinstance(string, bytes):
        string = ''.join(r'\x{:02x}'.format(byte) for byte in string)
        if size % 2:
            size -= 1
    return '\n'.join([prefix + line for line in textwrap.wrap(string, size)])

main()




