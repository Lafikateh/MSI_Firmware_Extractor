// C Standard Library Headers
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Program Headers
#include "Main.h"
#include "Files.h"
#include "Strings.h"

#define MSI_New_Image_Start_Location 0x1A3090
#define MSI_Old_Image_Start_Location 0x1A30A8

// Program Entry Point
int main(int argument_count, char* argument_list[])
{
	// Print program header
	puts("MSI UEFI Firmware Extractor v1.0.0");

	if (argument_count == 1)
	{
		// Print usage
		puts("Usage: input_file [output_file]");

		// Report success
		return 0;
	}
	else if (argument_count == 2 || argument_count == 3)
	{
		// Determine the source and target file path
		const char* source_path = argument_list[1];
		const char* target_path = NULL;
		if (argument_count == 3)
		{
			target_path = argument_list[2];
		}
		else
		{
			target_path = argument_list[1];
			target_path = c_replace_string_c(target_path, ".EXE", ".rom");
			target_path = c_replace_string_c(target_path, ".exe", ".rom");
		}

		// Check if the source file exists
		if (check_if_exists(source_path) == true)
		{
			// Attempt to read the source file
			data_buffer* source_buffer = read_file(source_path);
			if (source_buffer != NULL)
			{
				// Determine the firmware image offset
				unsigned long firmware_image_offset = 0;
				memcpy(&firmware_image_offset, source_buffer->data + MSI_New_Image_Start_Location, 4);
				if (firmware_image_offset < 1024)
				{
					// The new firmware image was invalid, use the older one
					memcpy(&firmware_image_offset, source_buffer->data + MSI_Old_Image_Start_Location, 4);

					// Check if atleast the old firmware image offset is valid
					if (firmware_image_offset < 1024)
					{
						// Print error
						printf("Unable to determine firmware image offset!");

						// Free the source file buffer
						free_buffer(source_buffer);

						// Return failure
						return 1;
					}
				}

				// Allocate and zero-fill the target data buffer
				unsigned long target_data_buffer_size = source_buffer->size - firmware_image_offset;
				unsigned char* target_data_buffer = calloc(target_data_buffer_size, sizeof(unsigned char));

				// Copy the UEFI Image to the target file buffer
				memcpy(target_data_buffer, source_buffer->data + firmware_image_offset, target_data_buffer_size);

				// Create the target file buffer
				data_buffer* target_buffer = create_buffer(target_data_buffer, target_data_buffer_size);

				// Attempt to write the modified file
				bool write_result = write_file(target_path, target_buffer);

				// Free the file buffers
				free_buffer(target_buffer);
				free_buffer(source_buffer);

				if (write_result == true)
				{
					return 0;
				}
				else
				{
					// Print error
					printf("Failed to write file \"%s\"", target_path);

					// Return failure
					return 1;
				}
			}
			else
			{
				// Print error
				printf("Failed to read file \"%s\"", source_path);

				// Return failure
				return 1;
			}
		}
		else
		{
			// Print error
			puts("Input file does not exist!");

			// Return failure
			return 1;
		}
	}
	else if (argument_count > 3)
	{
		// Print error
		puts("Too many arguments provided!");

		// Return failure
		return 1;
	}
}
