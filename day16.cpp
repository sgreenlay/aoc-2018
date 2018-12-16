
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <map>
#include <set>
#include <algorithm>

struct execution
{
    std::vector<int> registers_before;
    std::vector<int> instruction;
    std::vector<int> registers_after;
};

enum parser_state
{
    Parse_Registers_Before,
    Parse_Instruction,
    Parse_Registers_After,
    Parse_Newline
};

enum parser_mode
{
    Parse_InstructionsAndRegisters,
    Parse_Instructions
};

std::vector<int> instructionFromString(std::string string)
{
    std::vector<int> values;
    
    auto start = 0;
    auto end = string.find(" ", start);

    while (end != std::string::npos)
    {
        auto value = string.substr(start, end - start);
        values.push_back(std::stoi(value));

        start = end + 1;
        end = string.find(" ", start);
    }

    auto value = string.substr(start);
    values.push_back(std::stoi(value));

    return values;
}

std::vector<int> registersFromString(std::string string)
{
    std::vector<int> values;

    auto start = string.find("[") + 1;
    auto end = string.find(", ", start);

    if (end == std::string::npos)
    {
        end = string.find("]", start);
    }

    while (end != std::string::npos)
    {
        auto value = string.substr(start, end - start);
        values.push_back(std::stoi(value));

        start = end + 2;
        end = string.find(", ", start);

        if (end == std::string::npos)
        {
            end = string.find("]", start);
        }
    }

    return values;
}

std::vector<execution> executionsFromFile(const char * path)
{
    std::vector<execution> executions;

    std::ifstream input;
    input.open(path);

    parser_state state = Parse_Registers_Before;

    execution ex = {};

    std::string line;
    parser_mode mode = Parse_InstructionsAndRegisters;

    while (std::getline(input, line))
    {
        if (line.length() == 0)
        {
            if (state == Parse_Newline)
            {
                if (mode == Parse_InstructionsAndRegisters)
                {
                    state = Parse_Registers_Before;
                }
                else
                {
                    state = Parse_Instruction;
                }
            }
            else if (state == Parse_Registers_Before)
            {
                state = Parse_Instruction;
                mode = Parse_Instructions;
            }
            continue;
        }

        switch (state)
        {
        case Parse_Registers_Before:
            ex.registers_before = registersFromString(line);
            state = Parse_Instruction;
            break;
        case Parse_Instruction:
            ex.instruction = instructionFromString(line);
            if (mode == Parse_InstructionsAndRegisters)
            {
                state = Parse_Registers_After;
            }
            else
            {
                executions.push_back(ex);
                ex = {};
                state = Parse_Instruction;
            }
            break;
        case Parse_Registers_After:
            ex.registers_after = registersFromString(line);
            executions.push_back(ex);
            ex = {};
            state = Parse_Newline;
            break;
        default:
            abort();
            break;
        }
    }
    input.close();

    return executions;
}

// INSTRUCTION A B C

enum instruction {
    Instruction_ADDR = 1,
    Instruction_ADDI,
    Instruction_MULR,
    Instruction_MULI,
    Instruction_BANR,
    Instruction_BANI,
    Instruction_BORR,
    Instruction_BORI,
    Instruction_SETR,
    Instruction_SETI,
    Instruction_GTIR,
    Instruction_GTRI,
    Instruction_GTRR,
    Instruction_EQIR,
    Instruction_EQRI,
    Instruction_EQRR,
    Instruction_End
};

std::vector<int> executeInstruction(instruction in, int A, int B, int C, std::vector<int> registers_before)
{
    std::vector<int> registers_after = registers_before;

    switch (in) {

        // ddr (add register) 
        //    stores into register C the result of adding register A and register B.

        case Instruction_ADDR:
        {
            registers_after[C] = registers_before[A] + registers_before[B];
            break;
        }

        // addi(add immediate)
        //    stores into register C the result of adding register A and value B.

        case Instruction_ADDI:
        {
            registers_after[C] = registers_before[A] + B;
            break;
        }

        // mulr(multiply register)
        //    stores into register C the result of multiplying register A and register B.

        case Instruction_MULR:
        {
            registers_after[C] = registers_before[A] * registers_before[B];
            break;
        }

        // muli(multiply immediate)
        //    stores into register C the result of multiplying register A and value B.

        case Instruction_MULI:
        {
            registers_after[C] = registers_before[A] * B;
            break;
        }

        // banr(bitwise AND register)
        //    stores into register C the result of the bitwise AND of register A and register B.

        case Instruction_BANR:
        {
            registers_after[C] = registers_before[A] & registers_before[B];
            break;
        }

        // bani(bitwise AND immediate)
        //    stores into register C the result of the bitwise AND of register A and value B.

        case Instruction_BANI:
        {
            registers_after[C] = registers_before[A] & B;
            break;
        }

        // borr(bitwise OR register)
        //    stores into register C the result of the bitwise OR of register A and register B.

        case Instruction_BORR:
        {
            registers_after[C] = registers_before[A] | registers_before[B];
            break;
        }

        // bori(bitwise OR immediate)
        //    stores into register C the result of the bitwise OR of register A and value B.

        case Instruction_BORI:
        {
            registers_after[C] = registers_before[A] | B;
            break;
        }

        // setr(set register)
        //    copies the contents of register A into register C. (Input B is ignored.)

        case Instruction_SETR:
        {
            registers_after[C] = registers_before[A];
            break;
        }

        // seti(set immediate)
        //    stores value A into register C. (Input B is ignored.)

        case Instruction_SETI:
        {
            registers_after[C] = A;
            break;
        }

        // gtir(greater - than immediate / register)
        //    sets register C to 1 if value A is greater than register B. Otherwise, register C is set to 0.

        case Instruction_GTIR:
        {
            registers_after[C] = A > registers_before[B] ? 1 : 0;
            break;
        }

        // gtri(greater - than register / immediate)
        //    sets register C to 1 if register A is greater than value B. Otherwise, register C is set to 0.

        case Instruction_GTRI:
        {
            registers_after[C] = registers_before[A] > B ? 1 : 0;
            break;
        }

        // gtrr(greater - than register / register)
        //    sets register C to 1 if register A is greater than register B. Otherwise, register C is set to 0.

        case Instruction_GTRR:
        {
            registers_after[C] = registers_before[A] > registers_before[B] ? 1 : 0;
            break;
        }

        // eqir(equal immediate / register)
        //    sets register C to 1 if value A is equal to register B. Otherwise, register C is set to 0.

        case Instruction_EQIR:
        {
            registers_after[C] = A == registers_before[B] ? 1 : 0;
            break;
        }

        // eqri(equal register / immediate)
        //    sets register C to 1 if register A is equal to value B. Otherwise, register C is set to 0.

        case Instruction_EQRI:
        {
            registers_after[C] = registers_before[A] == B ? 1 : 0;
            break;
        }

        // eqrr(equal register / register)
        //    sets register C to 1 if register A is equal to register B. Otherwise, register C is set to 0.

        case Instruction_EQRR:
        {
            registers_after[C] = registers_before[A] == registers_before[B] ? 1 : 0;
            break;
        }
    }

    return registers_after;
}

std::set<instruction> possibleInstructionsFromExecution(execution ex)
{
    std::set<instruction> possibleInstructions;

    int A = ex.instruction[1];
    int B = ex.instruction[2];
    int C = ex.instruction[3];

    auto couldBeRegister = [](int V) {
        return (V >= 0 && V <= 3);
    };

    if (!couldBeRegister(C))
    {
        abort();
    }

    std::vector<instruction> instructionsToTry;

    if (couldBeRegister(A) && couldBeRegister(B))
    {
        instructionsToTry.push_back(Instruction_ADDR);
        instructionsToTry.push_back(Instruction_MULR);
        instructionsToTry.push_back(Instruction_BANR);
        instructionsToTry.push_back(Instruction_BORR);
        instructionsToTry.push_back(Instruction_GTIR);
        instructionsToTry.push_back(Instruction_GTRR);
        instructionsToTry.push_back(Instruction_EQRR);
    }

    if (couldBeRegister(A))
    {
        instructionsToTry.push_back(Instruction_ADDI);
        instructionsToTry.push_back(Instruction_MULI);
        instructionsToTry.push_back(Instruction_BANI);
        instructionsToTry.push_back(Instruction_BORI);
        instructionsToTry.push_back(Instruction_SETI);
        instructionsToTry.push_back(Instruction_SETR);
        instructionsToTry.push_back(Instruction_GTRI);
        instructionsToTry.push_back(Instruction_EQRI);
    }

    if (couldBeRegister(B))
    {
        instructionsToTry.push_back(Instruction_GTIR);
        instructionsToTry.push_back(Instruction_EQIR);
    }

    for (auto && ir : instructionsToTry)
    {
        auto registers_after = executeInstruction(ir, A, B, C, ex.registers_before);

        bool instructionMatch = true;
        for (int i = 0; i < registers_after.size(); ++i)
        {
            if (registers_after[i] != ex.registers_after[i])
            {
                instructionMatch = false;
                break;
            }
        }

        if (instructionMatch)
        {
            possibleInstructions.emplace(ir);
        }
    }

    return possibleInstructions;
}

int main()
{
    auto executions = executionsFromFile("data/day16.txt");

    int count = 0;

    std::map<int, std::set<instruction>> possibleInstructionValues;

    int i = 0;
    for (i = 0; i < executions.size(); ++i)
    {
        auto ex = executions[i];

        if (ex.registers_before.size() == 0)
        {
            break;
        }

        auto possibleInstructions = possibleInstructionsFromExecution(ex);
        //printf("%d possible instructions\n", possibleInstructions.size());

        if (possibleInstructions.size() >= 3)
        {
            count++;
        }

        int in = ex.instruction[0];
        if (possibleInstructionValues.count(in) == 0)
        {
            possibleInstructionValues.emplace(in, std::set<instruction>{ possibleInstructions });
        }
        else
        {
            for (auto instruction : possibleInstructions)
            {
                possibleInstructionValues[in].emplace(instruction);
            }
        }
    }

    printf("%d with 3 or more possible instructions\n", count);

    std::map<int, instruction> instructionValues;

    while (possibleInstructionValues.size())
    {
        std::vector<int> remove;
        
        for (auto possibleInstruction : possibleInstructionValues)
        {
            for (auto instruction : instructionValues)
            {
                possibleInstruction.second.erase(instruction.second);
            }

            if (possibleInstruction.second.size() == 1)
            {
                instructionValues[possibleInstruction.first] = *possibleInstruction.second.begin();
                remove.push_back(possibleInstruction.first);
            }
        }

        for (auto in : remove)
        {
            possibleInstructionValues.erase(in);
        }
    }

    std::vector<int> registers = { 0, 0, 0, 0 };
    for (; i < executions.size(); ++i)
    {
        auto ex = executions[i];

        registers = executeInstruction(
            instructionValues[ex.instruction[0]],
            ex.instruction[1],
            ex.instruction[2],
            ex.instruction[3],
            registers);
    }

    for (auto && r : registers)
    {
        printf("%d ", r);
    }
    printf("\n");

    std::getc(stdin);
}
