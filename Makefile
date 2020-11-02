all:
	$(CC) -std=c99 -Os -s -o MSI_Firmware_Extractor Main.c Files.c Strings.c

release:
	$(CC) -std=c99 -Os -s -o MSI_Firmware_Extractor Main.c Files.c Strings.c

debug:
	$(CC) -std=c99 -g3 -o MSI_Firmware_Extractor Main.c Files.c Strings.c

clean:
	rm -f $(OBJS) $(OUT)
