#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{

	struct udev_list_entry *devices, *dev_list_entry;
	struct udev_enumerate *enumerate;
	struct udev_device *dev;
	struct udev *udev;

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

		printf("path=%s\n", path);
		printf("sysname=%s\n", udev_device_get_sysname(dev));
		printf("syspath=%s\n", udev_device_get_syspath(dev));
		printf("actual_brightness: %s\n", actual_brightness);
		printf("max_brightness: %s\n", max_brightness);

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
			else {
				printf("unsupported \(%c)\n", c);
				break;
			}

			char new_brightness[1024];
			sprintf(new_brightness, "%d", current_brightness);

			printf("new_brightness=%s\n", new_brightness);

			if (!strcmp(new_brightness, max_brightness)) {
				printf("new_brightness is equal to max_brightness\n");
				break;
			}

			if (0 > udev_device_set_sysattr_value(dev, "brightness",
							      new_brightness)) {
				perror("udev_device_set_sysattr_value");
			}
		} while (c != 'q');
	}

	udev_enumerate_unref(enumerate);

	udev_unref(udev);

	return EXIT_SUCCESS;
}
