def sender():
    pass

def receiver():
    pass

if __name__ == '__main__':
    
    # Hamming Codes
    # Data Self Correction Codes (Simulation)
    # Data Bit - n bit input data

    d = input("Enter Data Bits: ")                  # Data Input
    data_bits = len(d)                                 # Data Input Length

    parity_bits = 0                                 # initialize number of parity bits
    while True:
        if (2 ** parity_bits) >= (parity_bits + data_bits + 1):
            break
        parity_bits++

    code_bits = data_bits + parity_bits
    



    
    pass
