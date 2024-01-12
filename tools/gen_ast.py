#!/usr/bin/python3

import sys


def define_type(file, basename, classname, fields):
    field_list = fields.split(', ')
    file.write(f'struct {classname} final : {basename} {{\n')

    # Fields
    for field in field_list:
        file.write(f'    {field};\n')
    file.write('\n')

    # Constructor
    file.write(f'    {classname} ({fields}) {{\n')
    for field in field_list:
        name = field.split(' ')[1]
        file.write(f'        this->{name} = {name};\n')
    file.write('    }\n')

    file.write('};\n')


def define_ast(output_dir, basename, types):
    path = f"{output_dir}/{basename.lower()}.hpp"

    with open(path, "w", encoding="utf-8") as f:
        # f.write('#include "literal.hpp"\n')
        # f.write('#include "token.hpp"\n')
        f.write('typedef struct Literal {} Literal;\n')
        f.write('typedef struct Token {} Token;\n')

        f.write('\n')
        f.write(f'typedef struct {basename} {{\n')
        f.write(f'}} {basename};\n')

        for type_ in types:
            classname = type_.split(':')[0].strip()
            fields = type_.split(':')[1].strip()
            define_type(f, basename, classname, fields)
            f.write('\n')


def main(args):
    if len(args) != 2:
        print(f"Usage: {args[0]} <output directory>")
        sys.exit(64)

    output_dir = args[1]
    define_ast(output_dir, "Expr", [
        "BinaryExpr:    Expr left, Token op, Expr right",
        "GroupingExpr:  Expr expression",
        "LiteralExpr:   Literal value",
        "UnaryExpr:     Token op, Expr right",
    ])


if __name__ == "__main__":
    main(sys.argv)
