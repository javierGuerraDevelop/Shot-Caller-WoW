read_file = "../../text_files/combat_log_large.txt"
write_file = "../../text_files/combat_log_write.txt"

try:
    with open(read_file, "r") as read_f, open(write_file, "a") as write_f:
        for line in read_f:
            write_f.write(line)

    print(
        f"Successfully copied contents from '{
            read_file}' to '{write_file}'."
    )

except FileNotFoundError:
    print(f"Error: The file '{read_file}' was not found.")
except Exception as e:
    print(f"An unexpected error occurred: {e}")
