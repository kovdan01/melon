# Server Configuration Files Guide

The Messenger consists of several separate standalone services, that can be run separately in different locations on different machines. As such, every service need it's own configuration file. 

## Format

All config files have the same structure: `m_config.yaml` is YAML format file placed at the service's parent folder.

## Structure

Every service has a number of required parameters provided. This file details the necessary parameters for every service. 
### Common for all services
Every service needs to specify its networking options. Every service needs to know the location of all other services. This information is provided in form of a list. Possible locations include `local` (using path to service) and `remote` (using port and IP).  I
```yaml
# required parameters
networking:
    protocol: ipv4
    ip: 0.0.0.0
    port: 6606
locations:
    -
	  service_kind: server-server
	  location_kind: remote
	  protocol: ipv4
      ip: 192.168.66.1
      port: 6607
    -
	  service_kind: client-server
	  location_kind: remote
	  protocol: ipv6
      ip: "2001:db8::1"
      port: 6608  
     -
	  service_kind: storage
	  location_kind: local
      path: path/to/storage/
     -
	  service_kind: storage
	  location_kind: remote
	  protocol: ipv6
      ip: "2001:db8::1"
      port: 6607  
     -
	  service_kind: message
	  location_kind: local
      path: path/to/message/
     -
	  service_kind: authentification
	  location_kind: local
      path: path/to/authentification/   
```
Note that in this example there are two storage services: a local and a remote one.
Every service type has a number of it's own requrired parameters, which are detailed below. I
### Server-Server Service
| Parameter key | Description | Allowed Values | Default |
|---------------|-------------|----------------|---------|
| `policy` |Networking Policy |`default` is the standard policy<br>… |`default` |

### Client-Server Service
| Parameter key | Description | Allowed Values | Default |
|---------------|-------------|----------------|---------|
| `policy` |Networking Policy |`default` is the standard policy<br>… |`default` |

### Storage Service
| Parameter key | Description | Allowed Values | Default |
|---------------|-------------|----------------|---------|
| `db-location`|Location of the database|path/to/db|
| `rdbms-type` |Type of RDBMS used |`MySQL`<br>`MS SQL Server`<br>…|`MySQL` |

### Message Service
| Parameter key | Description | Allowed Values | Default |
|---------------|-------------|----------------|---------|
||||
### Authentification Service
| Parameter key | Description | Allowed Values | Default |
|---------------|-------------|----------------|---------|
|`sasl-policy`|SASL policy|`digest-md5`<br>`plain`<br>…||
|`certificate`|Dictionary with certificate details|For `type`:<br>  - `domain`<br>- `organization`<br>For `expiration`:<br>dd.mm.yyyy<br>… ||
## Additional fields
 A customised service may need additional parametes: specify them in the same file.
```yaml
# required parameters
networking:
    protocol: ipv4
    ip: 0.0.0.0
    port: 6606
# <...>
# additional parameters
favourite-animal: dog
favourite-cuisine:
     - italian
     - russian
     - indian


```
