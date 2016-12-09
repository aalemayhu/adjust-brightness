#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

int set_brightness(struct udev_device *dev, char *brightness)
{
	return udev_device_set_sysattr_value(dev, "brightness", brightness);
}

int main(int argc, const char *argv[])
{
	struct udev_list_entry *devices, *dev_list_entry;
	struct udev_enumerate *enumerate;
	bool should_turn_off = false;
	struct udev_device *dev;
	struct udev *udev;

	udev = udev_new();
	if (!udev) {
		perror("udev_new");
		return EXIT_FAILURE;
	}

	if (argc > 1)
		should_turn_off = !strcmp(argv[1], "off");

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

		printf("path=%s\n", path);
		printf("sysname=%s\n", udev_device_get_sysname(dev));
		printf("syspath=%s\n", udev_device_get_syspath(dev));
		printf("actual_brightness: %s\n", actual_brightness);
		printf("max_brightness: %s\n", max_brightness);

		if (should_turn_off) {
			if (0 > set_brightness(dev, "0"))
				perror("set_brightness");
			break;
		}

		printf("supported actions + and -\n");
		char c;
		do {
			do {
				c = getchar();
			} while (c == ' ' || c == '\n');

			int current_brightness;
			current_brightness =
				atoi(udev_device_get_sysattr_value(dev,
								   "brightness"));

			if (c == '+')
				current_brightness++;
			else if (c == '-')
				current_brightness--;
			else
				break;

			char new_brightness[1024];
			sprintf(new_brightness, "%d", current_brightness);

			printf("new_brightness=%s\n", new_brightness);

			if (!strcmp(new_brightness, max_brightness)) {
				printf("new_brightness is equal to max_brightness\n");
				break;
			}

			if (0 > set_brightness(dev, new_brightness))
				perror("set_brightness");
		} while (c != 'q');
	}

	udev_enumerate_unref(enumerate);

	udev_unref(udev);

	return EXIT_SUCCESS;
}
