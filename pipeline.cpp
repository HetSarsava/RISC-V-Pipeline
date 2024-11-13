#include <bits/stdc++.h>

using namespace std;

int GPR[32] = {0};

//--------------------------------------------------------------------REGISTERS CLASS------------------------------------------------------------------------
// Register data
class Register {
    private:
        string name;
        int number;
    public:
        Register(string reg) {
            name = reg;
            number = stoi(name.substr(1));
        }
        string to_binary() {
            return bitset<5>(number).to_string();
        }
        int to_number() {
            return number;
        }     
};

//--------------------------------------------------------------------IMMEDIATE CLASS------------------------------------------------------------------------
// Calculates different immediate values
class Immediate {
    private:
        string immediate_value;
        int int_value;
        string binary_value;
        
    public:
        Immediate(string imm_value) : immediate_value(imm_value), int_value(stoi(immediate_value)) {
            binary_value = bitset<12>(int_value).to_string();
        }
        
        string imm_to_binary() {
            return binary_value;
        }
        
        int imm_to_int() {
            return int_value;
        }
        
        string imm_to_binary5() {
            return bitset<5>(int_value).to_string();
        }
        
        string imm_to_ret5() {
            return binary_value.substr(binary_value.size() - 5, 5);
        }
        
        string imm_ret_7() {
            return binary_value.substr(0, binary_value.size() - 5);
        }
        
        string immJ() {
            return bitset<20>(int_value).to_string();
        }
};

//--------------------------------------------------------------------INSTRUCTION TYPE CLASS------------------------------------------------------------------------
// Calculates different instruction types
class InstructionType {
    private:
        string opcode, rs1, rs2, rd, imm, func3, func7;

    public:
        InstructionType(string op, string r1, string r2, string d, string i, string f3, string f7)
            : opcode(op), rs1(r1), rs2(r2), rd(d), imm(i), func3(f3), func7(f7) {}

        string R_type() {
            Register rdReg(rd), r1Reg(rs1), r2Reg(rs2);
            return func7 + r2Reg.to_binary() + r1Reg.to_binary() + func3 + rdReg.to_binary() + opcode;
        }

        string I_type() {
            Register rdReg(rd), r1Reg(rs1);
            Immediate Im(imm);
            return Im.imm_to_binary() + r1Reg.to_binary() + func3 + rdReg.to_binary() + opcode;
        }

        string IL_type() {
            Register rdReg(rd), r1Reg(rs1);
            Immediate Im(imm);
            return func7 + Im.imm_to_binary5() + r1Reg.to_binary() + func3 + rdReg.to_binary() + opcode;
        }

        string IR_type() {
            Register rdReg(rd), r1Reg(rs1);
            Immediate Im(imm);
            return func7 + Im.imm_to_binary5() + r1Reg.to_binary() + func3 + rdReg.to_binary() + opcode;
        }

        string L_type() {
            Register rdReg(rd), r1Reg(rs1);
            Immediate Im(imm);
            return Im.imm_to_binary() + r1Reg.to_binary() + func3 + rdReg.to_binary() + opcode;
        }

        string B_type() {
            Register r2Reg(rs2), r1Reg(rs1);
            Immediate Im(imm);
            string s1 = Im.imm_ret_7(), s2 = Im.imm_to_ret5();
            return s1 + r2Reg.to_binary() + r1Reg.to_binary() + func3 + s2 + opcode;
        }

        string S_type() {
            Register r2Reg(rs2), r1Reg(rs1);
            Immediate Im(imm);
            return Im.imm_ret_7() + r2Reg.to_binary() + r1Reg.to_binary() + func3 + Im.imm_to_ret5() + opcode;
        }

        string J_type() {
            Register rdReg(rd);
            Immediate Im(imm);
            return Im.immJ() + rdReg.to_binary() + opcode;
        }
};

//--------------------------------------------------------------------ASSEMBLER------------------------------------------------------------------------
// Parsing as well as converting the instruction from words to binary
int debug_a = 0;
class InstructionConvert {
    private:
        unordered_map<string, pair<string, pair<string, string>>> instruction = {
            {"ADD", {"R", {"000", "0000000"}}},
            {"SUB", {"R", {"000", "0100000"}}},
            {"AND", {"R", {"111", "0000000"}}},
            {"OR", {"R", {"110", "0000000"}}},
            {"XOR", {"R", {"100", "0000000"}}},
            {"SLL", {"R", {"001", "0000000"}}},
            {"SRL", {"R", {"101", "0100000"}}},
            {"SLT", {"R", {"010", "0000000"}}},
            {"SLTU", {"R", {"011", "0000000"}}},
            {"MUL", {"R", {"000", "0000001"}}},
            {"ADDI", {"I", {"000", ""}}},
            {"SLLI", {"IL", {"001", "0000000"}}},
            {"SLTI", {"IL", {"010", "0000000"}}},
            {"SLTIU", {"IL", {"011", "0000000"}}},
            {"XORI", {"I", {"100", ""}}},
            {"SRLI", {"IR", {"101", "0000000"}}},
            {"SRAI", {"IR", {"101", "0100000"}}},
            {"ORI", {"I", {"110", ""}}},
            {"ANDI", {"I", {"111", ""}}},
            {"LB", {"L", {"000", ""}}},
            {"LH", {"L", {"001", ""}}},
            {"LW", {"L", {"010", ""}}},
            {"LBU", {"L", {"100", ""}}},
            {"LHU", {"L", {"101", ""}}},
            {"BEQ", {"B", {"000", ""}}},
            {"BNE", {"B", {"001", ""}}},
            {"BLT", {"B", {"100", ""}}},
            {"BGE", {"B", {"101", ""}}},
            {"SB", {"S", {"000", ""}}},
            {"SH", {"S", {"001", ""}}},
            {"SW", {"S", {"010", ""}}},
            {"SD", {"s", {"011", ""}}},
            {"JAL", {"J", {"", ""}}},
            {"LUI", {"U", {"", ""}}}
        };

        unordered_map<string, string> type_to_opcode = {
            {"R", "0110011"},
            {"I", "0010011"},
            {"IL", "0010011"},
            {"IR", "0010011"},
            {"L", "0000011"},
            {"B", "1100011"},
            {"S", "0100011"},
            {"J", "1101111"},
            {"U", "0110111"}
        };

    public:
        string convert(string str) {
            debug_a++;
            string opcode = "";
            int ind = 0;
            while (ind < str.size() && str[ind] != ' ') {
                opcode += str[ind];
                ind++;
            }
            ind++;
            debug_a++;
            if (instruction.find(opcode) == instruction.end()) {
                return "Instruction Invalid";
            }
            auto instruction_pair = instruction[opcode];
            string type = instruction_pair.first;
            string func3 = instruction_pair.second.first;
            string func7 = instruction_pair.second.second;
            debug_a++;
        if (type == "R") {
            debug_a++;
            string rd = "", r1 = "", r2 = "";
            int sw = 0;
            for (; ind < str.size(); ind++) {
                if (str[ind] == ' ') {
                    sw++;
                    continue;
                }
                if (sw == 0) {
                    rd += str[ind];
                } else if (sw == 1) {
                    r1 += str[ind];
                } else {
                    r2 += str[ind];
                }
            }
            if (!rd.empty() && rd.back() == ',') rd.pop_back();
            if (!r1.empty() && r1.back() == ',') r1.pop_back();

            opcode = type_to_opcode[type];
            InstructionType instruction_type(opcode, r1, r2, rd, "", func3, func7);
            return instruction_type.R_type();
            debug_a++;
        } else if (type == "I" || type == "IR" || type == "IL") {
            debug_a++;
            string rd = "", r1 = "", imm = "";
            int sw = 0;
            for (; ind < str.size(); ind++) {
                if (str[ind] == ' ') {
                    sw++;
                    continue;
                }
                if (sw == 0) {
                    rd += str[ind];
                } else if (sw == 1) {
                    r1 += str[ind];
                } else {
                    imm += str[ind];
                }
            }

            if (!rd.empty() && rd.back() == ',') rd.pop_back();
            if (!r1.empty() && r1.back() == ',') r1.pop_back();

            opcode = type_to_opcode[type];
            InstructionType instruction_type(opcode, r1, "", rd, imm, func3, func7);
            if (type == "I") return instruction_type.I_type();
            else if (type == "IR") return instruction_type.IR_type();
            else if (type == "IL") return instruction_type.IL_type();
            debug_a++;
        } else if (type == "L") {
            debug_a++;
            string rd = "", comb = "", imm = "", rs1 = "";
            while (ind < str.size() && str[ind] != ' ') {
                rd += str[ind++];
            }
            ind++;
            while (ind < str.size()) {
                comb += str[ind++];
            }
            if (!rd.empty() && rd.back() == ',') rd.pop_back();
            bool in_brackets = false;
            for (int i = 0; i < comb.size() - 1; i++) {
                if (comb[i] == '[') {
                    in_brackets = true;
                    i++;
                }
                if (in_brackets) {
                    rs1 += comb[i];
                } else {
                    imm += comb[i];
                }
            }

            opcode = type_to_opcode[type];
            InstructionType instruction_type(opcode, rs1, "", rd, imm, func3, "");
            return instruction_type.L_type();
            debug_a++;
        } else if (type == "B") {
            debug_a++;
            string r1 = "", r2 = "", imm = "";
            int sw = 0;
            for (; ind < str.size(); ind++) {
                if (str[ind] == ' ') {
                    sw++;
                    continue;
                }
                if (sw == 0) {
                    r1 += str[ind];
                } else if (sw == 1) {
                    r2 += str[ind];
                } else {
                    imm += str[ind];
                }
            }

            if (!r1.empty() && r1.back() == ',') r1.pop_back();
            if (!r2.empty() && r2.back() == ',') r2.pop_back();

            opcode = type_to_opcode[type];
            InstructionType instruction_type(opcode, r1, r2, "", imm, func3, "");
            return instruction_type.B_type();
            debug_a++;
        } else if (type == "S") {
            debug_a++;
            string r2 = "", comb = "", imm = "", r1 = "";
            while (ind < str.size() && str[ind] != ' ') {
                r2 += str[ind++];
            }
            ind++;
            while (ind < str.size()) {
                comb += str[ind++];
            }
            if (!r2.empty() && r2.back() == ',') r2.pop_back();
            bool in_brackets = false;
            for (int i = 0; i < comb.size() - 1; i++) {
                if (comb[i] == '[') {
                    in_brackets = true;
                    i++;
                }
                if (in_brackets) {
                    r1 += comb[i];
                } else {
                    imm += comb[i];
                }
            }

            opcode = type_to_opcode[type];
            InstructionType instruction_type(opcode, r1, r2, "", imm, func3, "");
            return instruction_type.S_type();
            debug_a++;
        }  else if (type == "J") {
            debug_a++;
            string rd = "", imm = "";
            while (ind < str.size() && str[ind] != ' ') {
                rd += str[ind++];
            }
            ind++;
            while (ind < str.size()) {
                imm += str[ind++];
            }
            if (!rd.empty() && rd.back() == ',') rd.pop_back();

            opcode = type_to_opcode[type];
            InstructionType instruction_type(opcode, "", "", rd, imm, "", "");
            return instruction_type.J_type();
            debug_a++;
        } else if (type == "U") {
            debug_a++;
            string rd = "", imm = "";
            while (ind < str.size() && str[ind] != ' ') {
                rd += str[ind++];
            }
            ind++;
            while (ind < str.size()) {
                imm += str[ind++];
            }
            if (!rd.empty() && rd.back() == ',') rd.pop_back();

            opcode = type_to_opcode[type];
            InstructionType instruction_type(opcode, "", "", rd, imm, "", "");
            return instruction_type.J_type();
            debug_a++;
        }
        return "Instruction type invalid!";
        debug_a = 0;
    }
};

//--------------------------------------------------------------------INSTRUCTION MEMORY------------------------------------------------------------------------
// Memory allocated to store instructions
class InstructionMemory {
private:
    vector<string> memory; 
    int base_address;       

public:
    InstructionMemory(int baseAddr = 0) : base_address(baseAddr) {}

    void add_instruction(const string& instruction) {
        memory.push_back(instruction);
    }

    string fetch_instruction(int address) const {
        int index = (address - base_address) / 4;
        if (index < 0 || index >= memory.size()) {
            return "00000000000000000000000000000000";
        }
        return memory[index]; 
    }

    int size() const {
        return memory.size();
    }
};

InstructionMemory instruction_mem;

//--------------------------------------------------------------------MEMORY------------------------------------------------------------------------
// Data memory
class Memory {
private:
    vector<int> memory;
public:
    Memory(size_t size) : memory(size) {}

    int load(int address) const {
        if (address < 0 || address >= memory.size()) {
            cout << "Error: Memory access out of range" << endl;
            return 0;
        }
        return memory[address];
    }

    void store(int address, int value) {
        if (address < 0 || address >= memory.size()) {
            cout << "Error: Memory access out of range" << endl;
            return;
        }
        memory[address] = value;
    }
};

Memory mem(1024);

int sign_extend(const string& binary, int bits) {
    if (binary.length() > bits) {
        cout << "Error: Binary string exceeds the specified bit width.\n";
        return 0;
    }
    int value = stoi(binary, nullptr, 2);
    if (value & (1 << (bits - 1))) {
        value -= (1 << bits);  // Apply two's complement to extend the sign
    }
    return value;
}

unordered_map<string, string> Operation =  {
    {"00000000000000", "ADD"},
    {"00000000100000", "SUB"},
    {"00001110000000", "AND"},
    {"00001100000000", "OR"},
    {"00001000000000", "XOR"},
    {"00000010000000", "SLL"},
    {"00001010100000", "SRL"},
    {"00000100000000", "SLT"},
    {"00000110000000", "SLTU"},
    {"00000000000001", "MUL"},
    {"00010000000000", "ADD"},
    {"00011000000000", "XOR"},
    {"00011100000000", "OR"},
    {"00011110000000", "AND"},
    {"00010010000000", "SLL"},
    {"00010100000000", "SLT"},
    {"00010110000000", "SLTU"},
    {"00011010100000", "SRL"},
    {"00011010000000","SRA"},
    {"00100000000000", "ADD"},
    {"00100010000000", "ADD"},
    {"00100100000000", "ADD"},
    {"00100000000000", "ADD"},
    {"00100010000000", "ADD"},
    {"00100100000000", "ADD"},
    {"00110000000000", "BEQ"},
    {"00110100000000", "BLE"},
    {"00110110000000", "BGT"},
    {"00110010000000", "BNE"},
    {"00111000000000", "BLT"},
    {"00111010000000", "BGE"},
    {"01010000000000","JAL"},
    {"01100000000000", "LUI"}
};

char new_line() {
    return '\n';
}

// This not actually parsing but initiating it
class StringParser {
    private:
        string temp;
    public:
    StringParser(string str) {
        temp = str;
    }
    void parse() {
        string st = "";
        for(int i=0; i<temp.size(); i++) {
            if(temp[i] == new_line()) {
                InstructionConvert converter;
                instruction_mem.add_instruction(converter.convert(st));
                st = "";
            } else {
                st+= temp[i];
            }
        }
    }
};

string ALUgenerate(string ALUop,string func3,string func7) {
    return Operation[ALUop + func3 + func7];
}

//--------------------------------------------------------------------ALU------------------------------------------------------------------------
int ALU(const string& operation, int operand1, int operand2) {
    if (operation == "ADD") return operand1 + operand2;
    if (operation == "SUB") return operand1 - operand2;
    if (operation == "AND") return operand1 & operand2;
    if (operation == "MUL") return operand1 * operand2;
    if (operation == "XOR") return operand1 ^ operand2;
    if (operation == "OR") return operand1 | operand2;
    if (operation == "SLL") return operand1 << operand2;
    if (operation == "SRL") return static_cast<unsigned int>(operand1) >> operand2;
    if (operation == "SRA") return operand1 >> operand2;
    if (operation == "SLT") return operand1 < operand2 ? 1 : 0;
    if (operation == "SLTU") return static_cast<unsigned int>(operand1) < static_cast<unsigned int>(operand2) ? 1 : 0;

    bool conditionMet = false;
    if (operation == "BEQ") conditionMet = (operand1 == operand2);
    else if (operation == "BNE") conditionMet = (operand1 != operand2);
    else if (operation == "BLT") conditionMet = (operand1 < operand2);
    else if (operation == "BGE") conditionMet = (operand1 >= operand2);
    else if (operation == "BLE") conditionMet = (operand1 <= operand2);
    else if (operation == "BGT") conditionMet = (operand1 > operand2);
    else {
        cout << "Unknown operation: " << operation << endl;
        return 0;
    }
    
    return conditionMet ? 1 : 0;
}

//--------------------------------------------------------------------IFID------------------------------------------------------------------------
class InstructionFetch {
    private:
        string instruction = "00000000000000000000000000000000";
        int npc = 0;
        int dpc = 0;
    public:
        void setCounters(string inst, int n, int d) {
            instruction = inst;
            npc = n;
            dpc = d;
        }
        int returnDPC() {
            return dpc;
        }
        int returnNPC() {
            return npc;
        }
        string getinstruction() {
            return instruction;
        }
        void print_content() {
        }
};

//--------------------------------------------------------------------IDEX------------------------------------------------------------------------
class InstructionDecodeExecute {
    private:
        string opcode = "0000000";
        string CW= "0000000000";
        string func3 = "000";
        string func7 = "0000000";
        int regd = 0;
        int reg1 = 0;
        int reg2 =0;
        int imm = 0;
        int DPC = 0;
        int NPC = 0;
        int JPC = 0;

        // CW stored against Opcode
        unordered_map<string, string> Control_word = {
            {"0110011", "00000000010"},  // R-type
            {"0010011", "00011000010"},  // I-type
            {"0000011", "00101001011"},  // Load
            {"1100011", "00110010000"},  // Branch
            {"0100011", "00101000100"},  // Store
            {"1101111", "01011100000"},  // Jump
            {"0110111", "01101000010"}   // LUI
        };
    public:

    void setRILS(string op, string f7, string f3, int r1, int r2, int rd, int npc, int i, int dpc) {
        opcode = op;
        func7 = f7;
        func3 = f3;
        reg1 = r1;
        reg2 = r2;
        regd = rd;
        NPC = npc;
        imm = i;
        DPC = dpc;
    }

    void setJ(string op, string f3, string f7, int i, int r1, int r2, int rd, int dpc, int jpc, int npc) {
        opcode = op;
        func7 = f7;
        func3 = f3;
        reg1 = r1;
        reg2 = r2;
        regd = rd;
        NPC = npc;
        DPC = dpc;
        JPC = jpc;
        imm = i;
    }

    void setB(string op, string f3, string f7, int r1, int r2, int i, int dpc, int npc) {
        opcode = op;
        func7 = f7;
        func3 = f3;
        reg1 = r1;
        reg2 = r2;
        imm = i;
        DPC = dpc;
        NPC = npc;
    }

    void setU(string op, int rg, int i, int dpc, int npc) {
        opcode = op;
        regd = rg;
        DPC = dpc;
        NPC = npc;
    }
    void calcCW() {
        CW = Control_word[opcode];
    }
    void setCW(string cw) {
        CW = cw;
    }
    
    int returnNPC() {
        return NPC;
    }
    string returnopcode() {
        return opcode;
    }
    string returnfunc3() {
        return func3;
    }
    string returnfunc7() {
        return func7;
    }
    int returnRegD() {
        return regd;
    }
    int returnReg1() {
        return reg1;
    }
    int returnReg2() {
        return reg2;
    }
    int returnImmediate() {
        return imm;
    }
    int returnDPC() {
        return DPC;
    }
    int returnJPC() {
        return JPC;
    }
    string returnCW() {
        return CW;
    }
    void print_content() {
        // cout<<"Opcode: "<<opcode<<endl;
        // cout<<"Func3: "<<func3<<endl;
        // cout<<"Func7: "<<func7<<endl;
        // cout<<"Register1: "<<reg1<<endl;
        // cout<<"Register2: "<<reg2<<endl;
        // cout<<"RegisterDestination: "<<regd<<endl;
        // cout<<"Immediate: "<<imm<<endl;
        // cout<<"DPC: "<<DPC<<endl;
        // cout<<"CW: "<<CW<<endl;
        // cout<<"JPC: "<<JPC<<endl;
    }
    void clean_house() {
        opcode = "0000000";
        CW= "0000000000";
        func3 = "000";
        func7 = "0000000";
        regd = 0;
        reg1 = 0;
        reg2 =0;
        imm = 0;
        DPC = 0;
        NPC = 0;
        JPC = 0;
    }
};

//--------------------------------------------------------------------EXMO------------------------------------------------------------------------
class ExecuteMemoryOperation {
    private:
        int ALUOPT = 0;
        string CW = "0000000000";
        int reg2 = 0;
        int exe = 0;
        int regd = 0;
    public:
        void setALUOPT(int value) {
            ALUOPT = value;
        }
        void setCW(string cw) {
            CW = cw;
        }
        void setReg2(const int r2) {
            reg2 = r2;
        }
        void setRegd(const int rd) {
            regd = rd;
        }
        void setExe(int ex) {
            exe = ex;
        }
        int returnALUOPT() {
            return ALUOPT;
        }
        string returnCW() {
            return CW;
        }
        int returnReg2() {
            return reg2;
        }
        int returnRegd() {
            return regd;
        }
        int returnExe() {
            return exe;
        }
};

//--------------------------------------------------------------------MOWB------------------------------------------------------------------------
class MemoryOperationWriteBack {   
    private:
        string CW = "0000000000";
        int LDOUT = 0;
        int ALUOUT = 0;
        int Regd = 0;
    public:
        void setCW(string cw) {
            CW = cw;
        }
        void setLDOUT(int value) {
            LDOUT = value;
        }
        void setALUOUT(int value) {
            ALUOUT = value;
        }
        void setRegd(int value) {
            Regd = value;
        }
        string returnCW() {
            return CW;
        }
        int returnLDOUT() {
            return LDOUT;
        }
        int returnALUOUT() {
            return ALUOUT;
        }
        int returnRegD() {
            return Regd;
        }
};

InstructionFetch ifid;
InstructionDecodeExecute idex;
ExecuteMemoryOperation exmo;
MemoryOperationWriteBack mowb;
int value = 0;
int prevBranch = 0;
int PC= 0;
int cycles = 0;

//--------------------------------------------------------------------FETCH------------------------------------------------------------------------
void Fetch() {
    cycles++;
    string res = instruction_mem.fetch_instruction(PC);
    ifid.setCounters(res, PC+4, PC);
    PC+=4;
    if(res=="00000000000000000000000000000000") {
        return;
    }
}

//--------------------------------------------------------------------DECODE------------------------------------------------------------------------

int debug1 = 0;
void Decode() {
    string str = ifid.getinstruction();
    int n = str.size();
    string opcode = str.substr(n-7, 7); 
    string prevopcode = opcode;
        if (opcode == "0110011") {
            // R
            debug1++;
            string func7, func3, r1, r2, rd;
            string operation = opcode + func3 + func7;
            debug1++;
            r1 = str.substr(n - 20, 5);
            r2 = str.substr(n - 25, 5);
            rd = str.substr(n - 12, 5);
            debug1++;
            string imm = "0";
            debug1++;
            idex.setRILS(prevopcode, str.substr(0, 7), str.substr(n - 15, 3), stoi(r1, 0, 2), stoi(r2, 0, 2), stoi(rd, 0, 2), ifid.returnNPC(), sign_extend(imm, 12), ifid.returnDPC());
            debug1++;
            idex.calcCW();
            exmo.setExe(1);
            debug1++;
        } else if (opcode == "0010011") {
            // I
            debug1++;
            string func3, r1, rd, imm, func7;
            func7 = str.substr(0, 7);
            string r2 = "00000";
            debug1++;
            if (func7 == "0000000" && (str.substr(n-15, 3) == "001"||str.substr(n-15,3)=="010"||str.substr(n-15,3)=="011"||str.substr(n-15,3)=="101")) {
                debug1++;
                func3 = str.substr(n - 15, 3);
                string operation = opcode + func3+func7;
                debug1++;
                r1 = str.substr(n - 20, 5); 
                imm = str.substr(7, 5);     
                rd = str.substr(n - 12, 5); 
                int shiftAmount = stoi(imm, 0, 2);
                debug1++;
                idex.setRILS(prevopcode, "0000000", func3, stoi(r1, 0, 2), stoi(r2, 0, 2), stoi(rd, 0, 2),ifid.returnNPC(), shiftAmount, ifid.returnDPC());
                idex.calcCW();
                exmo.setExe(1);
                debug1++;
            } else {
                debug1++;
                func3 = str.substr(n - 15, 3);
                string operation = opcode + func3;
                string r2 = "00000";
                string func7 = "0000000";
                debug1++;
                r1 = str.substr(n - 20, 5);
                imm = str.substr(0, 12);  
                rd = str.substr(n - 12, 5);
                operation.pop_back();
                debug1++;
                idex.setRILS(prevopcode, func7, func3, stoi(r1, 0, 2), stoi(r2, 0, 2), stoi(rd, 0, 2),ifid.returnNPC(), sign_extend(imm, 12), ifid.returnDPC());
                idex.calcCW();
                exmo.setExe(1);
                debug1++;
            }
            debug1++;
        } else if(opcode=="0000011") {
            // L
            debug1++;
            string func3,r1,rd,imm;
            func3 = str.substr(n - 15, 3);
            string operation = opcode + func3;
            r1 = str.substr(n - 20, 5);
            debug1++;
            string r2 = "0";
            string func7 = "0000000";
            imm = str.substr(0, 12);  
            rd = str.substr(n - 12, 5);
            debug1++;
            idex.setRILS(prevopcode, func7, func3, stoi(r1, 0, 2), stoi(r2, 0, 2), stoi(rd, 0, 2), ifid.returnNPC(), sign_extend(imm, 12), ifid.returnDPC());
            idex.calcCW();
            exmo.setExe(1);
            debug1++;
        } else if(opcode=="0100011") {
            // S
            debug1++;
            string func3,r1,rd,imm7, imm5,imm;
            func3 = str.substr(n - 15, 3);
            string operation = opcode + func3;
            r1 = str.substr(n - 20, 5);
            string r2 = "0";
            debug1++;
            imm7 = str.substr(0, 7);  
            imm5 = str.substr(n-12,5);
            rd = str.substr(n - 25, 5);
            string func7 = "0000000";
            imm = imm7+imm5;
            debug1++;
            idex.setRILS(prevopcode, func7, func3, stoi(r1, 0, 2), stoi(r2, 0, 2), stoi(rd, 0, 2), ifid.returnNPC(), sign_extend(imm, 12), ifid.returnDPC());
            idex.calcCW();
            exmo.setExe(1);
            debug1++;
        } else if(opcode=="1101111") {
            // J
            debug1++;
            string rd, imm;
            string r1 = "0";
            string r2 = "0";
            string func7 = "0000000";
            string func3 = "000";
            debug1++;
            rd = str.substr(n - 12, 5);
            imm = str.substr(0,20);
            int reg1 = stoi(r1, 0, 2);
            int reg2 = stoi(r2, 0, 2);
            int regd = stoi(rd, 0, 2);
            debug1++;
            int Immediate = sign_extend(imm,20);
            debug1++;
            idex.setJ(prevopcode, func3, func7, Immediate, stoi(r1, 0, 2), stoi(r2, 0, 2), stoi(rd, 0, 2), ifid.returnDPC(), ifid.returnNPC()+Immediate-4, ifid.returnNPC());
            idex.calcCW();
            exmo.setExe(1);
            debug1++;
            GPR[regd]= ifid.returnNPC()+Immediate-4;
            //Jump
            debug1++;
            if(idex.returnCW()[5]=='1') {
                int JPC = idex.returnJPC();
                PC = JPC;
                idex.setCW("0000000000");
                exmo.setExe(0);
            }
            debug1++;
        } else if(opcode=="1100011") {
            // B
            debug1++;
            string func3,r1,r2,imm5,imm7,imm;
            string func7 = "0000000";
            func3 = str.substr(n - 15, 3);
            string operation = opcode + func3;
            debug1++;
            r1 = str.substr(n - 20, 5);
            r2 = str.substr(n - 25, 5);
            imm5 = str.substr(n - 12, 5);
            imm7 = str.substr(0, 7);
            imm = imm7+imm5;
            debug1++;
            idex.setB(prevopcode, func7, func3, stoi(r1, 0, 2), stoi(r2, 0, 2), sign_extend(imm,12), ifid.returnDPC(), ifid.returnNPC());
            idex.calcCW();
            exmo.setExe(1);
            debug1++; 
            //Branch
            debug1++;
            int reg1val = GPR[idex.returnReg1()];
            int reg2val = GPR[idex.returnReg2()];
            debug1++;

            // Forwarding
            if((exmo.returnCW()[9]=='1') && (exmo.returnRegd()!=0) && idex.returnReg1()==exmo.returnRegd()) {   
                debug1++;
                reg1val = exmo.returnALUOPT();
            } else if((exmo.returnCW()[9]=='1') && (exmo.returnRegd()!=0) && idex.returnReg2()==exmo.returnRegd()) {
                debug1++;
                reg2val = exmo.returnALUOPT();
            } else if((mowb.returnCW()[9]=='1')&& (mowb.returnRegD()!=0)&&idex.returnReg1()==mowb.returnRegD()) {
                debug1++;
                if(mowb.returnCW().back()=='1') {
                    reg1val = mowb.returnLDOUT();
                } else {
                    reg1val = mowb.returnALUOUT();
                }
            } else if((mowb.returnCW()[9]=='1')&& (mowb.returnRegD()!=0)&& idex.returnReg2()==mowb.returnRegD()) {
                debug1++;
                if(mowb.returnCW().back()=='1') {
                    reg2val = mowb.returnLDOUT();
                } else {
                    reg2val = mowb.returnALUOUT();
                }
            }

            debug1++;
            string ALUselect = ALUgenerate(idex.returnCW().substr(0,4),idex.returnfunc3(),idex.returnfunc7());
            int ALU_res = ALU(ALUselect,reg1val,reg2val);
            int DPC = idex.returnDPC();
            int NPC = idex.returnNPC();
            int FPC = NPC;
            debug1++;
            if(idex.returnCW()[6]=='1' && ALU_res==1) {  
                debug1++;
                int BPC = DPC+idex.returnImmediate();
                idex.setCW("0000000000");
                PC = BPC;
                exmo.setExe(0);
                debug1++;
            } else {
                debug1++;
                PC = NPC;
            }
            debug1++;
            idex.setCW("0000000000");
            exmo.setExe(0);
            prevBranch = 1;
            debug1++;
        } else if(opcode=="0110111") {
            // U
            debug1++;
            string rd, imm;
            rd = str.substr(n - 12, 5);
            imm = str.substr(0,20);
            debug1++;
            idex.setU(prevopcode, stoi(rd, 0, 2), sign_extend(imm,20), ifid.returnDPC(), idex.returnNPC());
            idex.calcCW();
            exmo.setExe(1);
            debug1++;
        } else  {
            debug1++;
            // Invalid Opcode
            idex.clean_house();
        }
        debug1 = 0;
}

//--------------------------------------------------------------------EXECUTE------------------------------------------------------------------------
void Execute() {
    string controlWord = idex.returnCW();
    exmo.setCW(controlWord);

    // Check for pipeline stall condition
    if (controlWord[7] == '1' && (idex.returnReg1() == exmo.returnRegd() || idex.returnReg2() == exmo.returnRegd())) {
        exmo.setCW("00000000000");
        PC = idex.returnNPC() + 4;
        exmo.setExe(0);
        return;
    }

    int reg1Value = GPR[idex.returnReg1()];
    int reg2Value = (controlWord[4] == '1') ? idex.returnImmediate() : GPR[idex.returnReg2()];

    // Handle data hazards with forwarding
    if (!prevBranch) {
        if (exmo.returnCW()[9] == '1' && exmo.returnRegd() != 0) {
            if (idex.returnReg1() == exmo.returnRegd()) reg1Value = exmo.returnALUOPT();
            if (idex.returnReg2() == exmo.returnRegd()) reg2Value = exmo.returnALUOPT();
        }
        if (mowb.returnCW()[9] == '1' && mowb.returnRegD() != 0) {
            if (idex.returnReg1() == mowb.returnRegD()) reg1Value = value;
            if (idex.returnReg2() == mowb.returnRegD()) reg2Value = value;
        }
    } else {
        if (mowb.returnCW()[9] == '1' && mowb.returnRegD() != 0) {
            if (idex.returnReg1() == mowb.returnRegD()) reg1Value = value;
            if (idex.returnReg2() == mowb.returnRegD()) reg2Value = value;
        }
    }

    int nextPC = idex.returnNPC();
    string ALUOperation = ALUgenerate(controlWord.substr(0, 4), idex.returnfunc3(), idex.returnfunc7());
    exmo.setReg2(idex.returnReg2());
    exmo.setRegd(idex.returnRegD());

    int ALUResult = ALU(ALUOperation, reg1Value, reg2Value);
    
    if (exmo.returnExe() == 1) {
        PC = nextPC + 4;
    }
    exmo.setExe(0);
    exmo.setALUOPT(ALUResult);
}

//--------------------------------------------------------------------MEMORY OPERATION------------------------------------------------------------------------
void MemoryOperation() {
    string controlWord = exmo.returnCW();
    mowb.setCW(controlWord);
    mowb.setRegd(exmo.returnRegd());
    mowb.setALUOUT(exmo.returnALUOPT());

    bool memRead = (controlWord[7] == '1');
    bool memWrite = (controlWord[8] == '1');

    if (memRead) {
        mowb.setLDOUT(mem.load(exmo.returnALUOPT()));
    } else if (memWrite) {
        mem.store(exmo.returnALUOPT(), GPR[exmo.returnReg2()]);
    }
}

//--------------------------------------------------------------------WRITE BACK------------------------------------------------------------------------
void WriteBack() {
    string controlWord = mowb.returnCW();

    // Select value from either ALU output or load output based on control word
    value = (controlWord.back() == '1') ? mowb.returnLDOUT() : mowb.returnALUOUT();

    // Write value to register if specified by control word
    if (controlWord[9] == '1') {
        GPR[mowb.returnRegD()] = value;
    }
}


//--------------------------------------------------------------------CPU CLASS------------------------------------------------------------------------ 
class CentralProcessingUnit {
    public:
    void run() {
        while(1) {
            if(PC>(instruction_mem.size()+4)*4) {
                break;
            }
            WriteBack();
            MemoryOperation();
            Execute();
            Decode();
            idex.print_content();  
            Fetch();
            ifid.print_content();
        }
    }
};

CentralProcessingUnit cpu;

// Factorial of a given number say "5";
// string Input = R"(
//     ADDI x1, x1, 5
//     ADDI x2, x2, 1
//     BEQ x1, x0, 16
//     MUL x2, x2, x1
//     ADDI x1, x1, -1
//     JAL x3, -12
// )";
// Ans = 120

// Sum of N natural numbers say "10";
// string Input = R"(
//     ADDI x1, x1, 10
//     ADDI x2, x2, 0
//     BEQ x1, x0, 16
//     ADD x2, x2, x1
//     ADDI x1, x1, -1
//     JAL x3, -12
// )";
// Ans = 55

int main() {
    string str = "";
    string input;

    // 6 is for the above commented code
    int number_of_lines = 6;
    string temp;
    for (int i=0; i<number_of_lines; i++) {
        getline(cin, input);
        input += new_line();
        str += input;
    }
    StringParser final_str(str);
    final_str.parse();    
    cpu.run();
    cout<<"Output : "<<GPR[2]<<endl;
    cout<<"Cycles : "<<cycles<<endl;
    return 0;
}