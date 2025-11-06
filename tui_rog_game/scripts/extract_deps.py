import os
import re
import sys

def find_cmakelists_files(root_dir):
    """Finds all CMakeLists.txt files in a directory."""
    cmakelists_files = []
    for root, _, files in os.walk(root_dir):
        if ".git" in root:
            continue
        for file in files:
            if file == "CMakeLists.txt":
                cmakelists_files.append(os.path.join(root, file))
    return cmakelists_files

def parse_dependencies(cmakelists_files):
    """Parses dependencies from a list of CMakeLists.txt files."""
    dependencies = set()
    aliases = {}

    # First pass: find all aliases to map simple target names to namespaced names
    for file_path in cmakelists_files:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
            # Find aliases like: add_library(tui_rog_game::adapter::in::tui ALIAS adapter_in_tui)
            alias_matches = re.findall(r'add_library\((tui_rog_game::\S+)\s+ALIAS\s+(\S+)\)', content)
            for alias, target in alias_matches:
                aliases[target.strip(')')] = alias

    aliases['tui_rog_game'] = 'tui_rog_game'

    # Second pass: find dependencies
    for file_path in cmakelists_files:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
            # A regex to capture the target and the entire block of libraries
            tll_matches = re.finditer(r'target_link_libraries\s*\(([^)]+)\)', content, re.DOTALL)

            for match in tll_matches:
                block = match.group(1)
                # Remove comments
                block = re.sub(r'#.*', '', block)
                # Split by whitespace
                parts = block.split()
                
                if not parts:
                    continue
                
                target_name = parts[0]
                canonical_target = aliases.get(target_name)

                if not canonical_target:
                    continue

                # Now find all linked libraries in the rest of the parts
                current_scope = "PUBLIC" # CMake's default if not specified, though we are explicit in this project
                for part in parts[1:]:
                    if part in ["PUBLIC", "PRIVATE", "INTERFACE"]:
                        current_scope = part
                    elif part.startswith("tui_rog_game::"):
                        style = "solid"
                        if current_scope == "PRIVATE":
                            style = "dashed"
                        elif current_scope == "INTERFACE":
                            style = "dotted"
                        dependencies.add(f'"{canonical_target}" -> "{part}" [style={style}];')

    return sorted(list(dependencies))

def main():
    """Main function."""
    if len(sys.argv) < 2:
        print("Usage: python extract_deps.py <output_file> [project_root_directory]")
        sys.exit(1)

    output_file = sys.argv[1]
    project_root = sys.argv[2] if len(sys.argv) > 2 else os.getcwd()


    cmakelists_files = find_cmakelists_files(project_root)
    dependencies = parse_dependencies(cmakelists_files)

    with open(output_file, 'w', encoding='utf-8') as f:
        f.write("digraph G {\n")
        f.write("  rankdir=LR;\n")
        f.write('  label="TUI Roguelike Game Architecture";\n')
        f.write("  node [shape=box, style=rounded];\n")
        for dep in dependencies:
            f.write(f"  {dep}\n")
        f.write("}\n")

    print(f"Architecture graph written to {output_file}")

if __name__ == "__main__":
    main()
