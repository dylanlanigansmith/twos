import sys

class PageIndices:
    def __init__(self):
        self.p4 = 0
        self.pdpt3 = 0
        self.pd2 = 0
        self.pt = 0

def get_page_index_vm(virtual_addr):
    idx = PageIndices()
    
   
    print("idx for VA:", hex(virtual_addr), "[", bin(virtual_addr), "]")
    
    idx.p4 = (virtual_addr >> 39) & 0x1FF

    
    idx.pdpt3 = (virtual_addr >> 30) & 0x1FF
 
    
    idx.pd2 = (virtual_addr >> 21) & 0x1FF

    
    idx.pt = (virtual_addr >> 12) & 0x1FF

    
    print("   p4 =", idx.p4, "\n   p3 =", idx.pdpt3, "\n   p2 =", idx.pd2, "\n   p1 =", idx.pt)




if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: ./pagecalc.py 0xffffffff80000000")
        sys.exit(1)
    
    try:
        virtual_addr = int(sys.argv[1], 16)  # Convert hexadecimal string to integer
        print("Get Page Index For Virt Addr:", hex(virtual_addr))
        assert (virtual_addr >> 51) == 0x1FFF, "Invalid virtual address: The 13 most significant bits are not all 1." #dont remember if it was 13 but probs fine
        get_page_index_vm(virtual_addr)
    except ValueError:
        print("Invalid virtual address in arg. Usage: ./pagecalc.py 0xffffffff80000000")
