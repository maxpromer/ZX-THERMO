Blockly.JavaScript['max90614_object_temp'] = function(block) {
	var code = `DEV_I2C1.MLX90614(0, 0x5A).readObjectTempC()`;
	return [code, Blockly.JavaScript.ORDER_NONE];
};

Blockly.JavaScript['max90614_ambient_temp'] = function(block) {
	var code = `DEV_I2C1.MLX90614(0, 0x5A).readAmbientTempC()`;
	return [code, Blockly.JavaScript.ORDER_NONE];
};

