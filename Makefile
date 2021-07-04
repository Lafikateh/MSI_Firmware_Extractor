all:
	@$(CC) -std=c99 -Os -s -o MSI_Firmware_Extractor Main.c

release:
	@$(CC) -std=c99 -Os -s -o MSI_Firmware_Extractor Main.c

debug:
	@$(CC) -std=c99 -g3 -o MSI_Firmware_Extractor Main.c

clean:
	@$(RM) MSI_Firmware_Extractor
