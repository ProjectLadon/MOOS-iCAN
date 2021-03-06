# MOOS-iCAN
Linux CAN interface connector for MOOS. This provides an interface to any device supported by socketCAN.

# configuration

## CONFFILE
The path of a file containing a JSON string with the same format as ```CONF```

## CONF
A JSON string defining the CAN configuration and conforming to the following schema:
```
{
	"$schema": "http://json-schema.org/schema#",
	"id": "CAN_conf",
	"type": "object",
    "definitions": {
        "const_field": {
            "type": "object",
            "properties":{
				"type": {"type":"string", "enum": ["const"]},
                "start_byte": {
                    "type": "integer",
                    "minimum": 0,
                    "maximum": 8,
                },
                "value": {
                    "type": "array",
                    "items": [{
                        "type":"integer",
                        "minimum": 0,
                        "maximum": 255
                    }],
                    "minItems": 1,
                    "maxItems": 8
                }
            }
        },
        "uint8_field": {
            "type": "object",
            "properties": {
				"type": {"type":"string", "enum": ["uint8"]},
                "var": {"type": "string"},
                "start_byte": {
                    "type": "integer",
                    "minimum": 0,
                    "maximum": 8,
                }
            }
        },
        "uint16_field": {
            "type": "object",
            "properties": {
				"type": {"type":"string", "enum": ["uint16"]},
                "var": {"type": "string"},
                "start_byte": {
                    "type": "integer",
                    "minimum": 0,
                    "maximum": 7,
                }
            }
        },
        "uint32_field": {
            "type": "object",
            "properties": {
				"type": {"type":"string", "enum": ["uint32"]},
                "var": {"type": "string"},
                "start_byte": {
                    "type": "integer",
                    "minimum": 0,
                    "maximum": 5,
                }
            }
        },
        "uint64_field": {
            "type": "object",
            "properties": {
				"type": {"type":"string", "enum": ["uint64"]},
                "var": {"type": "string"}
            }
        },
        "int8_field": {
            "type": "object",
            "properties": {
				"type": {"type":"string", "enum": ["int8"]},
                "var": {"type": "string"},
                "start_byte": {
                    "type": "integer",
                    "minimum": 0,
                    "maximum": 8,
                }
            }
        },
        "int16_field": {
            "type": "object",
            "properties": {
				"type": {"type":"string", "enum": ["int16"]},
                "var": {"type": "string"},
                "start_byte": {
                    "type": "integer",
                    "minimum": 0,
                    "maximum": 7,
                }
            }
        },
        "int32_field": {
            "type": "object",
            "properties": {
				"type": {"type":"string", "enum": ["int32"]},
                "var": {"type": "string"},
                "start_byte": {
                    "type": "integer",
                    "minimum": 0,
                    "maximum": 5,
                }
            }
        },
        "int64_field": {
            "type": "object",
            "properties": {
				"type": {"type":"string", "enum": ["int64"]},
                "var": {"type": "string"}
            }
        },
        "float_field": {
            "type": "object",
            "properties": {
				"type": {"type":"string", "enum": ["float"]},
                "var": {"type": "string"},
                "start_byte": {
                    "type": "integer",
                    "minimum": 0,
                    "maximum": 5,
                }
            }
        },
        "double_field": {
            "type": "object",
            "properties": {
				"type": {"type":"string", "enum": ["double"]},
                "var": {"type": "string"}
            }
        },
        "string_field": {
            "type": "object",
            "properties": {
				"type": {"type":"string", "enum": ["string"]},
                "var": {"type": "string"},
                "start_byte": {
                    "type": "integer",
                    "minimum": 0,
                    "maximum": 8,
                },
                "length": {
                    "type": "integer",
                    "minimum": 1,
                    "maximum": 9,
                }
            }
        },
        "binary_field": {
            "type": "object",
            "properties": {
				"type": {"type":"string", "enum": ["binary"]},
                "var": {"type": "string"},
                "start_byte": {
                    "type": "integer",
                    "minimum": 0,
                    "maximum": 8,
                },
                "length": {
                    "type": "integer",
                    "minimum": 1,
                    "maximum": 9,
                }
            }
        },
        "CAN_tx_msg": {
            "type": "object",
            "properties": {
				"type": {"type":"string", "enum": ["transmit"]},
                "id": {
                    "type": "integer",
                    "minimum": 1,
                    "maximum": 2048
                },
                "fields": {
                    "type": "array",
                    "items": [
                        {"oneOf": [
                            {"$ref": "#/definitions/const_field"},
                            {"$ref": "#/definitions/uint8_field"},
                            {"$ref": "#/definitions/uint16_field"},
                            {"$ref": "#/definitions/uint32_field"},
                            {"$ref": "#/definitions/uint64_field"},
                            {"$ref": "#/definitions/int8_field"},
                            {"$ref": "#/definitions/int16_field"},
                            {"$ref": "#/definitions/int32_field"},
                            {"$ref": "#/definitions/int64_field"},
                            {"$ref": "#/definitions/float_field"},
                            {"$ref": "#/definitions/double_field"},
                            {"$ref": "#/definitions/string_field"},
                            {"$ref": "#/definitions/binary_field"}
                        ]}
                    ]
                },
				"required": ["type", "id", "fields"]
            }
        },
        "CAN_rx_msg": {
            "type": "object",
            "properties": {
				"type": {"type":"string", "enum": ["receive"]},
                "id": {
                    "type": "integer",
                    "minimum": 1,
                    "maximum": 2048
                },
                "fields": {
                    "type": "array",
                    "items": {
                        "oneOf": [
                            {"$ref": "#/definitions/uint8_field"},
                            {"$ref": "#/definitions/uint16_field"},
                            {"$ref": "#/definitions/uint32_field"},
                            {"$ref": "#/definitions/uint64_field"},
                            {"$ref": "#/definitions/int8_field"},
                            {"$ref": "#/definitions/int16_field"},
                            {"$ref": "#/definitions/int32_field"},
                            {"$ref": "#/definitions/int64_field"},
                            {"$ref": "#/definitions/float_field"},
                            {"$ref": "#/definitions/double_field"},
                            {"$ref": "#/definitions/string_field"},
                            {"$ref": "#/definitions/binary_field"}
                        ]
                    }
                },
				"required": ["type", "id", "fields"]
            }
        }
    },
	"properties": {
        "interface": {"type": "string"},
        "tx_messages": {
            "type": "array",
            "items": [
                {"$ref": "#/definitions/CAN_tx_msg"}
            ]
        },
        "rx_messages": {
            "type": "array",
            "items": [
                {"$ref": "#/definitions/CAN_rx_msg"}
            ]
        }
    },
    "required": ["interface"]
}
```
