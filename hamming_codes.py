class HammingPacket:
    def __init__(self, data):

        # original state
        self.original_data = data

        # initialize other states
        self.sender_code = None
        self.received_code = None
        self.corrected_code = None

        # correction coefficients (vectors)
        self.incorrection_coefficient = None        # noise vector applied during transmission
        self.correction_coefficient = None          # correction code developed by receiver

    def encode_data(self):

        # original data
        d = self.original_data
        data_bits = len(d)

        # number of parity bits
        parity_bits = 0
        while (2 ** parity_bits) < (parity_bits + data_bits + 1):
            parity_bits += 1

        # total bits
        code_bits = data_bits + parity_bits

        # code and data arrays
        data = [x == '1' for x in d]
        code = [None] * code_bits

        # load data bits
        n = 0
        data_bit_count = data_bits - 1
        for i in range(code_bits):
            if i == ((2 ** n) - 1):
                code[i] = None
                n += 1
            else:
                code[i] = data[data_bit_count]
                data_bit_count -= 1
        
        # calculating parity bits
        for n in range(parity_bits):
            parity_pos = 2 ** n
            xor_sum = False
        
            for i in range(code_bits):
                if (i + 1) & parity_pos:
                    if code[i] is None:
                        continue
                    xor_sum = xor_sum ^ code[i]
                    
            code[parity_pos - 1] = xor_sum

        # sender code
        self.sender_code = [1 if bit else 0 for bit in code]

    def send_data(self, corrupt_index = None):
        if self.sender_code is None:
            print("[Sender Error] Cannot send data. Please run encode_data() first.")
            return

        self.incorrection_coefficient = [0] * len(self.sender_code)
        
        if corrupt_index is not None:
            if 0 <= corrupt_index < len(self.sender_code):
                actual_array_index = len(self.sender_code) - 1 - corrupt_index
                self.incorrection_coefficient[actual_array_index] = 1
                print(f"[Channel Noise] Simulating transmission error at Faculty Index {corrupt_index}...")
            else:
                print(f"[Channel Warning] Corrupt index {corrupt_index} out of bounds.")

        # Create the stream to send out into the wild
        transit_stream = []
        for s_bit, error_bit in zip(self.sender_code, self.incorrection_coefficient):
            transit_stream.append(s_bit ^ error_bit)
            
        return transit_stream

    def receive_data(self, transmission_stream):
        if transmission_stream is None:
            print("[Receiver Error] Antenna detected a dead line (No data received).")
            return
            
        # Capture the stream coming off the wire
        self.received_code = list(transmission_stream)
        print("[Antenna] Bitstream captured and buffered successfully.")
    
    def correct_data(self):
        """
        Processes the received_code stream using Boolean Hamming checks.
        Calculates the correction_coefficient (syndrome) to pinpoint and fix 
        any single-bit transmission errors.
        """
        if self.received_code is None:
            print("[Receiver Error] No data to check. Run send_data() first.")
            return

        code_bits = len(self.received_code)
        
        # 1. Map the received integer list back into a clean Boolean checking state
        code = [bit == 1 for bit in self.received_code]
        
        # 2. Determine the number of parity bit check passes required
        parity_bits = 0
        while (2 ** parity_bits) < (code_bits + 1):
            parity_bits += 1
            
        # 3. Calculate the Correction Coefficient (Syndrome)
        self.correction_coefficient = 0
        
        for n in range(parity_bits):
            parity_pos = 2 ** n
            xor_sum = False
            
            for i in range(code_bits):
                if (i + 1) & parity_pos:
                    xor_sum = xor_sum ^ code[i]
            
            # If a parity group doesn't balance to even, accumulate the error position
            if xor_sum:
                self.correction_coefficient += parity_pos

        # 4. Error Correction Execution Phase
        if self.correction_coefficient == 0:
            print("[Receiver] Verification successful: Zero transmission faults detected.")
            self.corrected_code = list(self.received_code)
        else:
            # Convert internal bit position into human left-to-right Faculty Index
            faculty_error_index = code_bits - self.correction_coefficient
            print(f"[Receiver] Alert: Data corruption discovered at Faculty Index {faculty_error_index}!")
            
            # Heal the corrupted bit by flipping its state
            error_array_index = self.correction_coefficient - 1
            if 0 <= error_array_index < code_bits:
                code[error_array_index] = not code[error_array_index]
                print("[Receiver] Error correction matrix successfully applied.")
            
            # Save the restored state back as integers
            self.corrected_code = [1 if bit else 0 for bit in code]
    
    def decode_data(self):
        if self.corrected_code is None:
            print("[Decoder Error] Cannot parse payload. Run correct_data() first.")
            return

        code_bits = len(self.corrected_code)
        extracted_data_bits = []
        
        # 1. Filter out all indexing positions that map to powers of 2
        n = 0
        for i in range(code_bits):
            position = i + 1
            if position == (2 ** n):
                n += 1  # Skip parity slot locations
            else:
                extracted_data_bits.append(self.corrected_code[i])
                
        # 2. Restore right-to-left layout order to print cleanly left-to-right
        restored_bits = extracted_data_bits[::-1]
        
        # 3. Convert integer list back into the original raw data string
        final_string = "".join(str(bit) for bit in restored_bits)
        print(f"[Decoder] Extraction complete. Retrieved payload: '{final_string}'")
        return final_string


if __name__ == '__main__':
    print("==================================================")
    print("        HAMMING CODE OOP NETWORK SIMULATOR        ")
    print("==================================================\n")
    
    packet = HammingPacket("01101")
    
    # 1. Sender Encodes
    packet.encode_data()
    print(f"-> Sender Encoded Vector (Faculty View):  {packet.sender_code[::-1]}")
    
    # 2. Transmission (Data leaves sender and goes onto the wire)
    print("\n--- Simulating Noisy Transmission Line ---")
    wire_output = packet.send_data(corrupt_index=3) 
    
    # 3. Receiver Captures (Antenna grabs data from the wire)
    packet.receive_data(wire_output)
    print(f"-> Code Received by Antenna (Faculty View): {packet.received_code[::-1]}")
    
    # 4. Correction & Decoding Phase
    print("\n--- Receiver Processing Phase ---")
    packet.correct_data()
    print(f"-> Restored Output Vector   (Faculty View): {packet.corrected_code[::-1]}")
    
    print("\n--- Data Decoding Phase ---")
    extracted_payload = packet.decode_data()
    print("==================================================")