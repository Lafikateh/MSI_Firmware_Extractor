// C Standard Library Headers
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Image start location definitions
#define MSI_Image_Start_Location_Count 3
unsigned long MSI_Image_Start_Locations[] = { 0x1A3090, 0x1A30A8, 0x1A30B4 };

bool read_file(FILE* handle, void* buffer, unsigned int* size)
{
	if(handle != NULL && size != NULL)
	{
		if(buffer != NULL)
		{
			fseek(handle, 0, SEEK_END);
			*size = ftell(handle);
			fseek(handle, 0, SEEK_SET);
			fread(buffer, 1, *size, handle);
		}
		else
		{
			fseek(handle, 0, SEEK_END);
			*size = ftell(handle);
			fseek(handle, 0, SEEK_SET);
		}
		
		return true;
	}
	else
	{
		return false;
	}
}

// Program Entry Point
int main(int argument_count, char* argument_list[])
{
	// Print program header
	printf("MSI UEFI Firmware Extractor v1.0.2\n");

	if (argument_count == 1)
	{
		// Print usage
		printf("Usage: input_file [output_file]\n");

		// Report success
		return 0;
	}
	else if (argument_count == 2 || argument_count == 3)
	{
		// Determine the source and target file path
		char* source_path = argument_list[1];
		char* target_path = NULL;
		if (argument_count == 3)
		{
			target_path = argument_list[2];
		}
		else
		{
			target_path = strdup(argument_list[1]);
			int length = strlen(target_path);
			
			if(strcmp(target_path + (length - 3), "EXE") == 0)
			{
				strcpy(target_path + (length - 3), "rom");
			}
			else if(strcmp(target_path + (length - 3), "exe") == 0)
			{
				strcpy(target_path + (length - 3), "rom");
			}
		}

		// Attempt to open the source file
		FILE* source_file = fopen(source_path, "rb");
		if (source_file != NULL)
		{
			// Get the source file size
			unsigned int source_file_size = 0;
			read_file(source_file, NULL, &source_file_size);
			
			// Read the source file
			unsigned char* source_file_buffer = malloc(source_file_size);
			bool read_result = read_file(source_file, source_file_buffer, &source_file_size);
			if (read_result == true)
			{
				// Determine the firmware image offset
				unsigned long firmware_image_offset = 0;
				for(unsigned int locations_processed = 0; locations_processed < MSI_Image_Start_Location_Count; locations_processed++)
				{
					// Copy the firmware image offset from the source file
					memcpy(&firmware_image_offset, source_file_buffer + MSI_Image_Start_Locations[locations_processed], 4);
					
					// Check if the firmware offset is valid
					if(firmware_image_offset > 1024)
					{
						break;
					}
				}
				
				// Check if the firmware offset is valid
				if(firmware_image_offset > 1024)
				{
					// Allocate and zero-fill the target data buffer
					unsigned long target_file_size = source_file_size - firmware_image_offset;
					unsigned char* target_file_buffer = calloc(target_file_size, sizeof(unsigned char));

					// Copy the UEFI Image to the target file buffer
					memcpy(target_file_buffer, source_file_buffer + firmware_image_offset, target_file_size);

					// Write the modified file
					FILE* destination_file = fopen(target_path, "wb");
					fwrite(target_file_buffer, 1, target_file_size, destination_file);

					// Free the file buffers
					free(target_file_buffer);
					free(source_file_buffer);

					// Close the file handles
					fclose(destination_file);
					fclose(source_file);
					
					return 0;
				}
				else
				{
					// Print error
					printf("Unable to determine firmware image offset!");

					// Free the source file buffer
					free(source_file_buffer);

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
			printf("Input file does not exist!\n");

			// Return failure
			return 1;
		}
	}
	else if (argument_count > 3)
	{
		// Print error
		printf("Too many arguments provided!\n");

		// Return failure
		return 1;
	}
}
