
#include <elfio/elfio.hpp>
#include <iostream>
#include <iomanip>
#include <vector>
#include <math.h>

using namespace ELFIO;
using namespace std;


// edit as desired
#define PAGE_SIZE       ((uint32_t)0x1000)
#define STACK_SIZE      ((uint32_t)0x1000)

// specific to MIPS
#define RESET_VECTOR    ((uint32_t)0xbfc00000)
#define STACK_START     (((uint32_t)0x8000000)-STACK_SIZE)


uint8_t reset_routine[] = {
    0x3c, 0x1d, 0x80, 0x00, // lui $sp, 0x8000
    0x3c, 0x1e, 0x00, 0x00, // FILL: lui $fp, <entry point>
    0x37, 0xde, 0x00, 0x00, // FILL: ori $fp, $fp, <entry point>
    0x03, 0xc0, 0x00, 0x08  // jr $fp
};
#define SET_ENTRY(__ENTRY) { \
    reset_routine[6] |= (__ENTRY>>24)&0xFF; \
    reset_routine[7] |= (__ENTRY>>16)&0xFF; \
    reset_routine[10] |= (__ENTRY>>8)&0xFF; \
    reset_routine[11] |= (__ENTRY>>0)&0xFF; \
}



class page_table {
    public:
        const uint32_t page_size;
        const uint32_t page_offset_mask;
        bool has(const uint32_t & virtual_address) const {
            for (const auto & m : table)
                if (m.has(virtual_address))
                    return true;
            return false;
        }
        void add(const uint32_t & virtual_address) {
            table.emplace_back(*this, virtual_address&(~page_offset_mask), page_size*physical_page_index);
            physical_page_index++;
        }
        void print() const {
            for (const auto & m : table) {
                cout << hex << m.virtual_address << "->" << m.physical_address << endl;
            }
        }
        uint32_t size() const { return table.size(); }
        page_table(const uint32_t & page_size = 0x1000) : physical_page_index(0), page_size(page_size), page_offset_mask(page_size-1) { }
        uint32_t physical_address(const uint32_t & virtual_address) const {
            for (const auto & m : table) {
                if (m.has(virtual_address)) {
                    return (virtual_address&page_offset_mask)|m.physical_address;
                }
            }
            throw PageFault();
        }
        void export_to_file(const std::string & filename) const {
            std::ofstream ofs( filename );
            for (const auto & m : table) {
                ofs << hex << (m.virtual_address>>((uint32_t)log2(page_size))) << '\n';
            }
            ofs.close();
        }

    private:
        class PageFault { };
        struct vm_map {
            const page_table & t;
            uint32_t virtual_address;
            uint32_t physical_address;
            bool has(const uint32_t & virtual_address) const {
                return (virtual_address >= this->virtual_address) && ((virtual_address < (this->virtual_address+t.page_size)));
            }
            vm_map(const page_table & t, const uint32_t & virtual_address, const uint32_t & physical_address) : t(t), virtual_address(virtual_address), physical_address(physical_address) { }
        };
        vector<vm_map> table;
        uint32_t physical_page_index;
};



int main(int argc, char const *argv[]) {
    if ( argc != 2 ) {
        cout << "Usage: elf2mem <elf_file>" << endl;
        return 1;
    }

    // Load ELF data
    elfio elf;
    if ( !elf.load( argv[1] ) ) {
        cout << "Can't find or process ELF file " << argv[1] << endl;
        return 2;
    }


    // create page table
    page_table t(PAGE_SIZE);
    uint32_t address_to_add;
    // fill according to ELF
    for (const auto & s : elf.segments) {
        address_to_add = s->get_virtual_address();
        while (address_to_add < (s->get_virtual_address()+s->get_memory_size())) {
            if (!t.has(address_to_add))
                t.add(address_to_add);
            address_to_add += t.page_size;
        }
    }
    // fill stack
    address_to_add = STACK_START;
    while (address_to_add < (STACK_START+STACK_SIZE)) {
        if (!t.has(address_to_add))
            t.add(address_to_add);
        address_to_add += t.page_size;
    }
    // fill reset routine
    address_to_add = RESET_VECTOR;
    while (address_to_add < (RESET_VECTOR+sizeof(reset_routine))) {
        if (!t.has(address_to_add))
            t.add(address_to_add);
        address_to_add += t.page_size;
    }
    // export
    t.export_to_file("tlb.mem");


    // initialize the memory
    uint32_t memory_size = t.size()*t.page_size;
    uint8_t memory[memory_size];
    memset(memory, '\0', memory_size);
    // fill according to elf segments
    for (const auto & s : elf.segments) {
        uint32_t physical_address = t.physical_address(s->get_virtual_address());
        memcpy(memory+physical_address, s->get_data(), s->get_file_size());
    }
    // set entry point and fill reset routine
    SET_ENTRY(elf.get_entry());
    memcpy(memory+t.physical_address(RESET_VECTOR), reset_routine, sizeof(reset_routine));
    // export
    std::ofstream memory_ofs( "memory.mem" );
    for (uint32_t i = 0; i < memory_size; i+=4) {
        memory_ofs << setfill('0') << setw(2) << hex << +memory[i+0];
        memory_ofs << setfill('0') << setw(2) << hex << +memory[i+1];
        memory_ofs << setfill('0') << setw(2) << hex << +memory[i+2];
        memory_ofs << setfill('0') << setw(2) << hex << +memory[i+3];
        memory_ofs << (((i+4)%16) ? ' ' : '\n');
    }
    memory_ofs.close();


    return 0;
}
