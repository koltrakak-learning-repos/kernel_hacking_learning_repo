# Sysfs e kobjects

Finora è stato solo il kernel a poter parlare con noi tramite il kernel log; sysfs è un meccanismo che permette anche a noi di interagire con il kernel (e viceversa) tramite Kobjects


A kobject is an object of type `struct kobject`. Kobjects have:

- a name 
- a reference count
- a parent pointer 
    - allowing objects to be arranged into hierarchies
- a specific type
- and, usually, a representation in the sysfs virtual filesystem (indeed they are arranged into hierarchies)

Kobjects are generally not interesting on their own; instead, they are usually embedded within some other structure which contains the stuff the code is really interested in.

- A ktype is the type of object that embeds a kobject. Every structure that embeds a kobject needs a corresponding ktype. The ktype controls what happens to the kobject when it is created and destroyed.

A kset is a group of kobjects.

- These kobjects can be of the same ktype or belong to different ktypes. 
- The kset is the basic container type for collections of kobjects. 
- When you see a sysfs directory full of other directories, generally each of those directories corresponds to a kobject in the same kset.

Sysfs is a RAM-based filesystem. It provides a means to export to userspace kernel data structures, their attributes, and the relationships between them.

- **sysfs is tied inherently to the kobject infrastructure**
- sysfs is compiled in if CONFIG_SYSFS is defined. You can access it by doing: `mount -t sysfs sysfs /sys`
    - sysfs è tipicamente già montato e si trova in /sys

**For every kobject that is registered with the system, a directory is created for it in sysfs**

- that directory is created as a subdirectory of the kobject’s parent, expressing internal object hierarchies to userspace. 
- top-level directories in sysfs represent the common ancestors of object hierarchies; i.e. the subsystems the objects belong to.

## Creating and removing simple kobjects

To create a simple directory in the sysfs hierarchy, use:

```struct kobject *kobject_create_and_add(const char *name, struct kobject *parent);```

- this function will create a kobject and place it in sysfs in the location underneath the specified parent

To create simple attributes associated with this kobject, use:

```int sysfs_create_file(struct kobject *kobj, const struct attribute *attr);```

or

```int sysfs_create_group(struct kobject *kobj, const struct attribute_group *grp);```

After a kobject has been registered, it must be cleaned up when the code is finished with it. 

- to do that, call `kobject_put()`
- by doing this, the kobject core will automatically clean up all of the memory allocated by this kobject

- if you need to do a two-stage delete of the kobject (say you are not allowed to sleep when you need to destroy the object), then call `kobject_del()` which will unregister the kobject from sysfs. 
    - This makes the kobject “invisible”, but it is not cleaned up, and the reference count of the object is still the same. At a later time call kobject_put() to finish the cleanup of the memory associated with the kobject.
- kobject_del() can be used to drop the reference to the parent object, if **circular references are constructed**
    - It is valid in some cases, that a parent objects references a child. 
    - Circular references _must_ be broken with an explicit call to kobject_del(), so that a release functions will be called, and the objects in the former circle release each other.

## Kobject attributes

Notevolmente ai Kobject possono essere associati uno o più attributi

NB: Gli attributi di un kobject sono rappresentati come file contenuti all'interno della directory del kobject corrispondente in /sys

- l'utente legge e scrive questi file per interagire con il kernel 
    - lettura/scrittura dei file attributo scatena un hook/callback (puntatore a funzione registrato in precedenza) all'interno del kernel che gestisce l'evento
- sysfs forwards file I/O operations to kernel methods defined for the attributes, **providing a means to read and write kernel attributes**
- Attributes should be ASCII text files, preferably with only one value per file. 

I file che rappresentano attributi sono file speciali/pseudofile

- leggere questi file ritorna una stringa
- scrivere questi file aggiorna questa stringa mandandola verso il kernel
- di conseguenza, **un attributo va semplicemente pensato come una stringa** che si può leggere/scrivere da userspace
    - i diritti di lettura/scrittura sono configurabili

TODO: scrivere/leggere questi attributi è una maniera per manipolare le strutture dati del kernel e per scatenare eventi nel kernel da userspace (verifica se questo è vero)

### Implementazione di un attributo

An attribute definition is simply:

```
// proprio simile ad un file
struct attribute {
    char            *name;
    struct module   *owner;
    umode_t         mode; // read, write, execute permissions
};

int sysfs_create_file(struct kobject * kobj, const struct attribute * attr);
void sysfs_remove_file(struct kobject * kobj, const struct attribute * attr);
```

A bare attribute contains no means to read or write the value of the attribute. Subsystems are encouraged to define their own attribute structure and wrapper functions for adding and removing attributes for a specific object type.

For example, the driver model defines struct device_attribute like:

```
struct device_attribute {
        struct attribute attr;
        ssize_t (*show)(struct device *dev, struct device_attribute *attr, char *buf);
        ssize_t (*store)(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
};

int device_create_file(struct device *, const struct device_attribute *);
void device_remove_file(struct device *, const struct device_attribute *);
```

For the common cases `/linux/sysfs.h` provides convenience macros to make defining attributes easier as well as making code more concise and readable. 

For example, this:

```
static struct device_attribute dev_attr_foo = {
        .attr = {
                .name = "foo",
                .mode = S_IWUSR | S_IRUGO,
        },
        .show = show_foo,
        .store = store_foo,
};
```

could be shortened to: ```static struct device_attribute dev_attr_foo = __ATTR_RW(foo);```