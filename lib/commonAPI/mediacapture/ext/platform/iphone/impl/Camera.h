
#import "ICamera.h"
#import "CameraBase.h"

@interface Camera : CameraBase<ICamera> {
}

-(void) takePicture:(NSDictionary*)propertyMap methodResult:(id<IMethodResult>)methodResult;




@end