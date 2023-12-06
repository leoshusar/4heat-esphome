import esphome.config_validation as cv

datapoint = cv.All(cv.string, cv.Length(min=6, max=6))

data = cv.All(cv.string, cv.Length(min=1, max=12))

message = cv.All(cv.string, cv.Length(min=18, max=18))

def ensure_dict(value):
    if not isinstance(value, dict):
        raise cv.Invalid("expected a dictionary")
    
    for k, v in value.items():
        if not k.isnumeric():
            raise cv.Invalid("expected an integer for key {}".format(k))
        
        if not isinstance(v, str):
            raise cv.Invalid("expected a string for key {}".format(k))
    
    return value
