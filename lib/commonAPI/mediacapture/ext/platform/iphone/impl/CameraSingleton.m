
#import "CameraSingleton.h"
#import "CameraBase.h"

extern int get_camera_max_width(const char* camera_type);
void camera_choose_picture(NSDictionary* options, id<IMethodResult> callback_api);
extern void save_image_to_device_gallery(const char* image_path);

@implementation CameraSingleton


-(NSString*)getInitialDefaultID {
    return CAMERA_TYPE_BACK;
}


-(void) enumerate:(id<IMethodResult>)methodResult {
    NSMutableArray* ar = [NSMutableArray arrayWithCapacity:2];
    if (get_camera_max_width([CAMERA_TYPE_BACK UTF8String]) > 0) {
        [ar addObject:CAMERA_TYPE_BACK];
    }
    if (get_camera_max_width([CAMERA_TYPE_FRONT UTF8String]) > 0) {
        [ar addObject:CAMERA_TYPE_FRONT];
    }
    [methodResult setResult:ar];
}

-(void) getCameraByType:(NSString*)cameraType methodResult:(id<IMethodResult>)methodResult {
    if (get_camera_max_width([cameraType UTF8String]) > 0) {
        [methodResult setResult:cameraType];
    }
}


-(void) choosePicture:(NSDictionary*)propertyMap methodResult:(id<IMethodResult>)methodResult {
    camera_choose_picture([CameraBase applyAliasesToDictionary:propertyMap], methodResult);
}

-(void) saveImageToDeviceGallery:(NSString*)pathToImage {
    save_image_to_device_gallery([pathToImage UTF8String]);
}


@end