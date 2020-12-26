import os
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("source_file", type=str, help="Shader source file")

    args = parser.parse_args()

    with open(args.source_file, "r") as f:
        lines = f.readlines()
        for line in lines:
            print('"{}\\n"'.format(line.replace("\n", "")))
