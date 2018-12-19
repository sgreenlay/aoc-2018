
#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <functional>

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

instruction instructionFromString(std::string str)
{
    if (str == "addr")
    {
        return Instruction_ADDR;
    }
    if (str == "addi")
    {
        return Instruction_ADDI;
    }
    if (str == "mulr")
    {
        return Instruction_MULR;
    }
    if (str == "muli")
    {
        return Instruction_MULI;
    }
    if (str == "banr")
    {
        return Instruction_BANR;
    }
    if (str == "bani")
    {
        return Instruction_BANI;
    }
    if (str == "borr")
    {
        return Instruction_BORR;
    }
    if (str == "bori")
    {
        return Instruction_BORI;
    }
    if (str == "setr")
    {
        return Instruction_SETR;
    }
    if (str == "seti")
    {
        return Instruction_SETI;
    }
    if (str == "gtir")
    {
        return Instruction_GTIR;
    }
    if (str == "gtri")
    {
        return Instruction_GTRI;
    }
    if (str == "gtrr")
    {
        return Instruction_GTRR;
    }
    if (str == "eqir")
    {
        return Instruction_EQIR;
    }
    if (str == "eqri")
    {
        return Instruction_EQRI;
    }
    if (str == "eqrr")
    {
        return Instruction_EQRR;
    }

    abort();
}

struct assembly_instruction
{
    instruction in;
    int A;
    int B;
    int C;
};

struct execution_environment
{
    int instruction_pointer;
    std::vector<assembly_instruction> instructions;
};

execution_environment environmentFromFile(const char * path)
{
    execution_environment environment;

    std::ifstream input;
    input.open(path);

    std::string line;
    std::getline(input, line);

    auto start = line.find("#ip ") + 4;
    auto end = 0;

    environment.instruction_pointer = std::stoi(line.substr(start));

    while (std::getline(input, line))
    {
        assembly_instruction assembly;
        
        start = 0;
        end = line.find(" ");

        assembly.in = instructionFromString(line.substr(start, end - start));

        start = end + 1;
        end = line.find(" ", start);

        assembly.A = std::stoi(line.substr(start, end - start));

        start = end + 1;
        end = line.find(" ", start);

        assembly.B = std::stoi(line.substr(start, end - start));

        start = end + 1;

        assembly.C = std::stoi(line.substr(start));

        environment.instructions.push_back(assembly);
    }
    input.close();

    return environment;
}

void instructionToC(int line, instruction in, int A, int B, int C, int ip)
{
    auto print_assignment_or_goto = [ip, line](int r) {
        if (r == ip)
        {
            printf("goto 1 + ");
        }
        else
        {
            printf("%c = ", 'a' + r);
        }
    };

    auto print_variable_or_value = [ip, line](int r) {
        if (r == ip)
        {
            printf("%d", line);
        }
        else
        {
            printf("%c", 'a' + r);
        }
    };

    printf("line_%d: \n", line);

    switch (in) {

        // ddr (add register) 
        //    stores into register C the result of adding register A and register B.

        case Instruction_ADDR:
        {
            printf("  ");
            print_assignment_or_goto(C);
            print_variable_or_value(A);
            printf(" + ");
            print_variable_or_value(B);
            printf(";\n");
            break;
        }

        // addi(add immediate)
        //    stores into register C the result of adding register A and value B.

        case Instruction_ADDI:
        {
            printf("  ");
            print_assignment_or_goto(C);
            print_variable_or_value(A);
            printf("+ %d ;\n", B);
            break;
        }

        // mulr(multiply register)
        //    stores into register C the result of multiplying register A and register B.

        case Instruction_MULR:
        {
            printf("  ");
            print_assignment_or_goto(C);
            print_variable_or_value(A);
            printf(" * ");
            print_variable_or_value(B);
            printf(";\n");
            break;
        }

        // muli(multiply immediate)
        //    stores into register C the result of multiplying register A and value B.

        case Instruction_MULI:
        {
            printf("  ");
            print_assignment_or_goto(C);
            print_variable_or_value(A);
            printf("* %d ;\n", B);
            break;
        }

        // banr(bitwise AND register)
        //    stores into register C the result of the bitwise AND of register A and register B.

        case Instruction_BANR:
        {
            printf("  ");
            print_assignment_or_goto(C);
            print_variable_or_value(A);
            printf(" & ");
            print_variable_or_value(B);
            printf(";\n");
            break;
        }

        // bani(bitwise AND immediate)
        //    stores into register C the result of the bitwise AND of register A and value B.

        case Instruction_BANI:
        {
            printf("  ");
            print_assignment_or_goto(C);
            print_variable_or_value(A);
            printf("& %d ;\n", B);
            break;
        }

        // borr(bitwise OR register)
        //    stores into register C the result of the bitwise OR of register A and register B.

        case Instruction_BORR:
        {
            printf("  ");
            print_assignment_or_goto(C);
            print_variable_or_value(A);
            printf(" | ");
            print_variable_or_value(B);
            printf(";\n");
            break;
        }

        // bori(bitwise OR immediate)
        //    stores into register C the result of the bitwise OR of register A and value B.

        case Instruction_BORI:
        {
            printf("  ");
            print_assignment_or_goto(C);
            print_variable_or_value(A);
            printf("| %d ;\n", B);
            break;
        }

        // setr(set register)
        //    copies the contents of register A into register C. (Input B is ignored.)

        case Instruction_SETR:
        {
            printf("  ");
            print_assignment_or_goto(C);
            print_variable_or_value(A);
            printf(";\n");
            break;
        }

        // seti(set immediate)
        //    stores value A into register C. (Input B is ignored.)

        case Instruction_SETI:
        {
            printf("  ");
            print_assignment_or_goto(C);
            printf("%d;\n", A);
            break;
        }

        // gtir(greater - than immediate / register)
        //    sets register C to 1 if value A is greater than register B. Otherwise, register C is set to 0.

        case Instruction_GTIR:
        {
            printf("  if (%d > ", A);
            print_variable_or_value(B);
            printf(") {\n", A);
            print_assignment_or_goto(C);
            printf("1;\n");
            printf("  } else {\n");
            print_assignment_or_goto(C);
            printf("0;\n");
            printf("  }\n");
            break;
        }

        // gtri(greater - than register / immediate)
        //    sets register C to 1 if register A is greater than value B. Otherwise, register C is set to 0.

        case Instruction_GTRI:
        {
            printf("  if (");
            print_variable_or_value(A);
            printf(" > %d) {\n", B);
            print_assignment_or_goto(C);
            printf("1;\n");
            printf("  } else {\n");
            print_assignment_or_goto(C);
            printf("0;\n");
            printf("  }\n");
            break;
        }

        // gtrr(greater - than register / register)
        //    sets register C to 1 if register A is greater than register B. Otherwise, register C is set to 0.

        case Instruction_GTRR:
        {
            printf("  if (");
            print_variable_or_value(A);
            printf(" > ");
            print_variable_or_value(B);
            printf(") {\n");
            print_assignment_or_goto(C);
            printf("1;\n");
            printf("  } else {\n");
            print_assignment_or_goto(C);
            printf("0;\n");
            printf("  }\n");
            break;
        }

        // eqir(equal immediate / register)
        //    sets register C to 1 if value A is equal to register B. Otherwise, register C is set to 0.

        case Instruction_EQIR:
        {
            printf("  if (%d == ", A);
            print_variable_or_value(B);
            printf(") {\n", A);
            print_assignment_or_goto(C);
            printf("1;\n");
            printf("  } else {\n");
            print_assignment_or_goto(C);
            printf("0;\n");
            printf("  }\n");
            break;
        }

        // eqri(equal register / immediate)
        //    sets register C to 1 if register A is equal to value B. Otherwise, register C is set to 0.

        case Instruction_EQRI:
        {
            printf("  if (");
            print_variable_or_value(A);
            printf(" == %d) {\n", B);
            print_assignment_or_goto(C);
            printf("1;\n");
            printf("  } else {\n");
            print_assignment_or_goto(C);
            printf("0;\n");
            printf("  }\n");
            break;
        }

        // eqrr(equal register / register)
        //    sets register C to 1 if register A is equal to register B. Otherwise, register C is set to 0.

        case Instruction_EQRR:
        {
            printf("  if (");
            print_variable_or_value(A);
            printf(" == ");
            print_variable_or_value(B);
            printf(") {\n");
            print_assignment_or_goto(C);
            printf("1;\n");
            printf("  } else {\n");
            print_assignment_or_goto(C);
            printf("0;\n");
            printf("  }\n");
            break;
        }
    }
}

int main()
{
    auto environment = environmentFromFile("data/day19.txt");

    // Part 1
    /*
    {
        std::vector<int> registers = { 0, 0, 0, 0, 0, 0 };
        for (int i = 0; i < environment.instructions.size(); ++i)
        {
            registers[environment.instruction_pointer] = i;

            auto assembly = environment.instructions[i];
            registers = executeInstruction(assembly.in, assembly.A, assembly.B, assembly.C, registers);

            printf("ip=%d [ ", i);
            for (auto && r : registers)
            {
                printf("%d ", r);
            }
            printf("]\n");

            i = registers[environment.instruction_pointer];
        }

        printf("\n[ ");
        for (auto && r : registers)
        {
            printf("%d ", r);
        }
        printf("]\n");
    }
    */


    for (int r = 0; r < 6; ++r)
    {
        printf("int %c = 0;\n", 'a' + r);
    }
    printf("\n");

    int i = 0;
    for (auto in : environment.instructions)
    {
        instructionToC(i++, in.in, in.A, in.B, in.C, environment.instruction_pointer);
    }

    std::getc(stdin);
}
