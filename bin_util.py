def print_bytes_context(file1, file2, pos, context=10):
    with open(file1, 'rb') as f1, open(file2, 'rb') as f2:
        data1 = f1.read()
        data2 = f2.read()
    
    start = max(0, pos - context)
    end = min(len(data1), pos + context)
    
    print(f"\nBytes from {start} to {end}:")
    print("Pos\tFile1\tFile2\tDiff?")
    print("-" * 30)
    
    for i in range(start, end):
        byte1 = data1[i]
        byte2 = data2[i]
        diff = "***" if byte1 != byte2 else ""
        print(f"{i}\t{byte1:02x}\t{byte2:02x}\t{diff}")

print_bytes_context('../listings/41_mine', '../listings/listing_41', 200)
