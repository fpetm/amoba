import sys
def main():
    sys.stdout.write("#include \"inf_am.hpp\"\nstd::vector<Position> g_Offsets[] = {{}, \n");
    for d in range(1,5+1):
        sys.stdout.write("{")
        for i in range(0,3**d):
            if i == (3**d-1)/2: continue
            sys.stdout.write(f"Position({d},{{")
            for j in range(d):
                sys.stdout.write(str(1-(i//3**j)%3) + ',')
            sys.stdout.write("}),")
        sys.stdout.write("},\n")
    sys.stdout.write("};")
    sys.stdout.flush()
main()
