#ifndef __NATIVE_GPIO_H
#define __NATIVE_GPIO_H

typedef int gpio_id_t;

#define GPID(i,p) (((i)*32)+p)

int gpio_request (gpio_id_t gpio);
int gpio_config (gpio_id_t gpio);
int gpio_set_direction (gpio_id_t gpio, const char *direction);
int gpio_read (gpio_id_t gpio);
int gpio_write (gpio_id_t gpio, int value);
int gpio_write_led (int led, int value);
int gpio_release (gpio_id_t gpio);
int gpio_request_input (gpio_id_t gpio);
int gpio_request_output (gpio_id_t gpio);

#endif /* __NATIVE_GPIO_H */
