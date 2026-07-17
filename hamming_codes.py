def sender(d):
    data_bits = len(d)                                 # Data Input Length

    parity_bits = 0                                 # number of parity bits
    while (2 ** parity_bits) < (parity_bits + data_bits + 1):
        parity_bits += 1

    code_bits = data_bits + parity_bits     # sender's code
    
    data = [x == '1' for x in d]                  # data bits array, using boolean logic rather than integers
    code = [None] * code_bits                       # sender code array
    parity_bit_count = 0
    data_bit_count = data_bits - 1

    # generating code without parity bits (all parity = 2 initially)
    n = 0
    for i in range(code_bits):
        if (i == ((2**n)-1)):
            code[i] = None
            parity_bit_count += 1
            n += 1
        else:
            code[i] = data[data_bit_count]
            data_bit_count -= 1

    # code = code[::-1]         code is in reverse order, but order can be ignored at this stage and formulated at the end
    #debug print, initial code structure
    #print(code) 

    # calculating parity bits
    for n in range(parity_bits):
        parity_pos = 2 ** n
        xor_sum = False
        
        # Scan all positions in the code array
        for i in range(code_bits):
            # Check if this position is covered by the current parity bit
            # (i + 1 because Hamming positions are 1-indexed natively)
            if (i + 1) & parity_pos:
                if code[i] == None:
                    continue
                xor_sum = xor_sum ^ code[i]
                
        # Assign the computed parity bit to its structural position
        code[parity_pos - 1] = xor_sum
    
    return code

def receiver(received_code_ints):
    code_bits = len(received_code_ints)
    
    # 1. Convert the incoming integer transmission back to Booleans for processing
    code = [bit == 1 for bit in received_code_ints]
    
    # 2. Determine how many parity bits are embedded in this message size
    parity_bits = 0
    while (2 ** parity_bits) < (code_bits + 1):
        parity_bits += 1
        
    error_position = 0
    
    # 3. Check every parity group
    for n in range(parity_bits):
        parity_pos = 2 ** n
        xor_sum = False
        
        # Scan the array to see if this parity group's bits balance out to False (0)
        for i in range(code_bits):
            if (i + 1) & parity_pos:
                xor_sum = xor_sum ^ code[i]
        
        # If xor_sum evaluates to True (1), it means this parity group found a mismatch!
        if xor_sum:
            error_position += parity_pos

    # 4. Error Correction Phase
    if error_position == 0:
        print("\n[Receiver] Check complete: No errors detected in transmission.")
    else:
        # Check if the error position falls within our actual array size
        if error_position <= code_bits:
            print(f"\n[Receiver] Mismatch detected at Hamming position: {error_position} (Array index {error_position - 1})")
            # Correct the bit by flipping its boolean state
            code[error_position - 1] = not code[error_position - 1]
            print("[Receiver] Error successfully corrected.")
        else:
            print(f"\n[Receiver] Multiple critical errors detected. (Position {error_position} out of bounds)")

    # 5. Convert back to the industry/human-readable integer format requested
    final_corrected_ints = [1 if bit else 0 for bit in code]
    return final_corrected_ints

if __name__ == '__main__':
    
    # Hamming Codes
    # Data Self Correction Codes (Simulation)
    # Data Bit - n bit input data

    d = input("Enter Data: ")                  # Data Input
    code = sender(d)

    # human readable print
    int_form = [1 if (bit == True) else 0 for bit in code]
    print("Sender Code: ",int_form)

    error_code = input("Enter Corresponding Error Code: ")
    corrected_code = receiver(error_code)
    print("Corrected Code: ", corrected_code)