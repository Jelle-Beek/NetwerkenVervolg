import logging

import azure.functions as func


def main(req: func.HttpRequest) -> func.HttpResponse:
    logging.info('Python HTTP trigger function processed a request.')

    brightness = req.params.get('brightness')
    if not brightness:
        try:
            req_body = req.get_json()
        except ValueError:
            pass
        else:
            brightness = req_body.get('brightness')

    if brightness:
        if int(brightness) < 500:
            return func.HttpResponse("It's dark, turn light ON")
        else: 
            return func.HttpResponse("It's bright, turn light OFF")

    else:
        return func.HttpResponse(
             "No useful variables detected, use 'brightness' for this page to do something.",
             status_code=200
        )
