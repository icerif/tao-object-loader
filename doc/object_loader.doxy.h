/**
 * @defgroup ObjectLoader Object Loader
 * @ingroup Modules
 *
 * Loads 3D models from files.
 *
 * With this module, you can import 3D models exported from many popular
 * 3D software packages.
 * Supported formats include:
 *   @li OBJ (*.obj)
 *   @li 3DS (*.3ds)
 *   @li Collada V1.4 (*.dae)
 *   @li 3DXML ASCII V3 and V4 (*.3dxml)
 *   @li STL (ASCII and binary) (*.stl)
 *   @li OFF and COFF (*.off)
 *
 * For instance, the following Tao document will load a 3D model from
 * file @c conestoga.3ds, and show it rotating around the z axis.
 @code
import ObjectLoader 1.0

the_light X, Y, Z ->
    light 0
    light_ambient 0.4, 0.4, 0.4, 1.0
    light_diffuse 0.4, 0.4, 0.4, 1.0
    light_specular 0.8, 0.8, 0.8, 1.0
    light_attenuation 1.0, 0.0, 0.0
    light_position X, Y, Z

rotating_obj F ->
    locally
        rotatex -70
        rotatez 8 * time
        object 0, 0, 0, 400, 400, 400, F

the_light 300, 300, 300
rotating_obj "conestoga.3ds"
 @endcode
 *
 * Here is a screen capture:
 * @image html conestoga.png "(3D model by Helmut Schaub &mdash; http://www.3dxtra.net, http://www.3d-xtra.bravepages.com)"
 * @{
 */

/**
 * Loads a 3D object from @a file.
 * No scaling or translation occurs: the model is drawn as defined in the file.
 */
object(text file);

/**
 * Loads and translates a 3D object.
 * @a file is a relative or absolute path to the model file.
 * The object is shown centered at (@a x, @a y, @a z).
 * No scaling occurs: the object dimensions are as defined in the model file.
 */
object(real x, real y, real z, text file);


/**
 * Loads, scales and translates a 3D object.
 * @a file is a relative or absolute path to the model file.
 * The object is shown centered at (@a x, @a y, @a z). "
 * The size of the object is specified by @a width, @a height and @a depth:
 * the model is scaled proportionally so that it fits in a box of the
 * specified size.
 */
object(real x, real y, real z, real width, real height, real depth, text file);

/**
 * Loads, translates and paints a 3D object.
 * Similar the the @ref object primitive, except that it forces the object to
 * take the current color or material properties even if the model file defines
 * colors, materials or textures.
 */
colored_object(real x, real y, real z, text file);


/**
 * Loads, scales, translates and paints a 3D object.
 * Similar the the @ref object primitive, except that it forces the object to
 * take the current color or material properties even if the model file defines
 * colors, materials or textures.
 */
colored_object(real x, real y, real z, real width, real height, real depth, text file);


/**
 * @}
 */
