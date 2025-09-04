# GIF Analyser

This is a command-line utility, developed in C++, designed to parse, decode, and display the structural information of non-animated GIF image files. The application reads a specified GIF file and outputs a detailed breakdown of its components, including header information, color tables, and the decompressed image data, directly to the console.

## Core Functionality

* **Header Parsing:** Reads and displays the GIF signature and version from the file header.
* **Logical Screen Descriptor:** Decodes and presents data from the Logical Screen Descriptor, including canvas dimensions and global color table information.
* **Global Color Table Processing:** Reads and prints the contents of the Global Color Table if one is present in the file.
* **Image Descriptor Decoding:** Parses the Image Descriptor block to extract image dimensions and local color table flags.
* **LZW Decompression:** Reads the table-based image data, processes the LZW-compressed byte stream to generate a code stream, and subsequently decodes this into an index stream.
* **Image Data Output:** Displays the final image data by presenting the Red, Green, and Blue color channels separately.

## Compilation and Execution

### System Requirements

* A C++11 standard compliant compiler (e.g., MinGW for Windows).

### For Windows (using MinGW)

1.  Open Command Prompt or PowerShell, navigate to the directory containing `a1.cpp`, and execute the following command to compile the application:
    ```
    g++ GIF_Analyser.cpp -o GIF_Analyser.exe -std=c++11
    ```
2.  To run the program, use the following command:
    ```
    GIF_Analyser.exe
    ```

## Operational Workflow

Upon execution, the program will prompt for user input:

1.  **Enter file name:**
    * The user must input the complete filename of the GIF to be processed (e.g., `squares.gif`).
2.  The application will then proceed to read the file and print the decoded structural and image data to the console before terminating.
