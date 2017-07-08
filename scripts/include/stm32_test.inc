#pragma library stm32_test

const leds
{
LED1=0,
LED2=1,
}

const states
{
OFF=0,
ON=1,
}

native led_control(leds: led, states: state);
