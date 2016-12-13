#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

int set_brightness(struct udev_device *dev, char *value)
{
	return udev_device_set_sysattr_value(dev, "brightness", value);
}

int main(int argc, const char *argv[])
{
	struct udev_list_entry *devices, *dev_list_entry;
	struct udev_enumerate *enumerate;
	const char *new_brightness;
	struct udev_device *dev;
	bool verbose = false;
	struct udev *udev;
	int i;

	if (2 > argc) {
		printf("%s: Please provide a value to use.\n", argv[0]);
		return EXIT_FAILURE;
	}
	new_brightness = argv[1];

	for (i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "-v")) {
			verbose = true;
			printf("verbose mode on\n");
			printf("new_brightness will be %s\n", new_brightness);
		}
	}

	udev = udev_new();
	if (!udev) {
		perror("udev_new");
		return EXIT_FAILURE;
	}

	enumerate = udev_enumerate_new(udev);

	udev_enumerate_add_match_subsystem(enumerate, "backlight");
	udev_enumerate_scan_devices(enumerate);
	devices = udev_enumerate_get_list_entry(enumerate);

	udev_list_entry_foreach(dev_list_entry, devices) {
		const char *path, *actual_brightness, *max_brightness;

		path = udev_list_entry_get_name(dev_list_entry);
		dev = udev_device_new_from_syspath(udev, path);
		actual_brightness = udev_device_get_sysattr_value(dev, "actual_brightness");
		max_brightness = udev_device_get_sysattr_value(dev, "max_brightness");

		if (verbose) {
			printf("path=%s\n", path);
			printf("sysname=%s\n", udev_device_get_sysname(dev));
			printf("syspath=%s\n", udev_device_get_syspath(dev));
			printf("actual_brightness: %s\n", actual_brightness);
			printf("max_brightness: %s\n", max_brightness);
		}

		if (!strcmp(new_brightness, max_brightness)) {
			if (verbose)
				printf("new_brightness is equal to max_brightness\n");
			break;
		}

		if (0 > set_brightness(dev, (char *)new_brightness))
			perror("set_brightness");
	}

	udev_enumerate_unref(enumerate);

	udev_unref(udev);

	return EXIT_SUCCESS;
}
