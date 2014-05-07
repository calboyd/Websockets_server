#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libwebsockets.h>
#include <json_spirit.h>

static int callback_http( struct libwebsocket_context *context,
								  struct libwebsocket *wsi,
								  enum libwebsocket_callback_reasons reason,
								  void *user,
								  void *in,
								  size_t len )
{
	switch ( reason )
	{
		case LWS_CALLBACK_ESTABLISHED:
			printf( "Connection established.\n" );
			break;
		case LWS_CALLBACK_CLIENT_WRITEABLE:
			printf( "Connection established.\n" );
			break;
		case LWS_CALLBACK_PROTOCOL_INIT:
			printf( "Protocol initialization.\n" );
			break;
		case LWS_CALLBACK_HTTP:
			{
				char *requested_uri = ( char * ) in;
				printf( "Requested URI: %s\n", requested_uri );
				
				if ( strcmp( requested_uri, "/" ) == 0 )
				{
					char *universal_response = ( char * ) "Hello, World!\n";
					
					libwebsocket_write( wsi, ( unsigned char * ) universal_response,
											  strlen( ( const char * ) universal_response ), LWS_WRITE_HTTP );
											  
					break;
				}
				else 
				{
					char cwd[ 1024 ];
					char *resource_path;
					
					if ( getcwd( cwd, sizeof( cwd ) ) != NULL )
					{
						resource_path = ( char * ) malloc( strlen( cwd ) + strlen( requested_uri ) );
						
						sprintf( resource_path, "%s%s", cwd, requested_uri );
						printf( "Resource path: %s\n", resource_path );
						
						char *extension = strrchr( resource_path, '.' );
						char *mime;
						
						if ( extension == NULL )
						{
							mime = "text/plain";
						}
						else if ( strcmp( extension, ".png" ) == 0 )
						{
							mime = "image/png";
						}
						else if ( strcmp( extension, ".jpg" ) == 0 )
						{
							mime = "image/jpg";
						}
						else if ( strcmp( extension, ".gif" ) == 0 )
						{
							mime = "image/gif";
						}
						else if ( strcmp( extension, ".html" ) == 0 )
						{
							mime = "image/html";
						}
						else if ( strcmp( extension, ".css" ) == 0 )
						{
							mime = "image/css";
						}
						else
						{
							mime = "text/plain";
						}
						
						libwebsockets_serve_http_file( context, wsi, resource_path, mime, NULL );
					}
				}
				
//				libwebsocket_close_and_free_session( context, wsi,
//																 LWS_CLOSE_STATUS_NORMAL );
				
				break;
			}
			case LWS_CALLBACK_HTTP_BODY:
				printf( "HTTP request data is now available.\n" );
				break;
			case LWS_CALLBACK_HTTP_BODY_COMPLETION:
				printf( "HTTP request data has been delivered.\n" );
				break;
			case LWS_CALLBACK_RECEIVE:
				printf( "Data has arrived.\n" );
				break;
			case LWS_CALLBACK_HTTP_WRITEABLE:
				printf( "HTTP link is writeable.\n" );
				break;
			case LWS_CALLBACK_CLOSED_HTTP:
				printf( "HTTP session has ended.\n" );
				break;
			case LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED:
				printf( "New client has been instantiated.\n" );
				break;
			case LWS_CALLBACK_HTTP_FILE_COMPLETION:
				printf( "HTTP file has been delivered.\n" );
				break;
		   case LWS_CALLBACK_GET_THREAD_ID:
//		   	printf( "Thread ID requested.\n" );
		   	break;
		   case LWS_CALLBACK_ADD_POLL_FD:
		   	printf( "Added Poll file descriptor.\n" );
		   	break;
		   case LWS_CALLBACK_LOCK_POLL:
		   	printf( "Lock Poll.\n" );
		   	break;
		   case LWS_CALLBACK_UNLOCK_POLL:
		   	printf( "Unlock Poll.\n" );
		   	break;
			default:
				printf( "Unhandled callback\n" );
				printf( "Reason: %d\n", reason );
				break;
	}

	return 0;
}

static int callback_ws( struct libwebsocket_context *context,
								  struct libwebsocket *wsi,
								  enum libwebsocket_callback_reasons reason,
								  void *user,
								  void *in,
								  size_t len )
{
	json_spirit::mObject Obj;
	std::string jsonString;
	
	Obj[ "name" ] = json_spirit::mValue( "Calvin" );	

	jsonString = json_spirit::write( Obj );

	switch ( reason )
	{
		case LWS_CALLBACK_PROTOCOL_INIT:
			printf( "Received the protocol init callback.\n" );
			break;
		case LWS_CALLBACK_ESTABLISHED:
			printf( "Received LWS_CALLBACK_ESTABLISHED.\n" );
			libwebsocket_callback_on_writable(context, wsi);
			break;
		case LWS_CALLBACK_CLOSED:
			printf( "Received LWS_CALLBACK_CLOSED.\n" );
			break;
		case LWS_CALLBACK_SERVER_WRITEABLE:
			{
			printf( "Sending Data.\n" );
			char *universal_response = "Hello, World!\n";
//			libwebsocket_write( wsi, ( unsigned char * ) universal_response,
//									  strlen( ( const char * ) universal_response ), LWS_WRITE_TEXT );
			libwebsocket_write( wsi, ( unsigned char * ) jsonString.c_str(),
									  jsonString.size(), LWS_WRITE_TEXT );
			libwebsocket_callback_on_writable(context, wsi);
			}
			break;
		case LWS_CALLBACK_RECEIVE:
//			printf( "Received LWS_CALLBACK_RECEIVE.\n" );
			break;
		default:
			printf( "Unhandled callback.\n" );
			printf( "Reason: %d\n", reason );
			break;
	}
	
	return 0;
}

static struct libwebsocket_protocols protocols[] = {
	{
		"http-only",
		callback_http,
		0
	},
	{
		"ws-only",
		callback_ws,
		0
	},
	{
		NULL,
		NULL,
		0
	}
};

int main( void )
{
	int port = 8080;
	const char *interface = NULL;
	struct libwebsocket_context *context;
	struct lws_context_creation_info context_info;
	
	memset( &context_info, 0, sizeof( struct lws_context_creation_info ) );
	context_info.port = 8080;
	context_info.iface = NULL;
	context_info.protocols = protocols;
	context_info.extensions = libwebsocket_get_internal_extensions();
	context_info.ssl_cert_filepath = NULL;
	context_info.ssl_private_key_filepath = NULL;
	context_info.ssl_ca_filepath = NULL;
	context_info.ssl_cipher_list = NULL;
	context_info.http_proxy_address = NULL;
	context_info.http_proxy_port = 0;
	context_info.gid = -1;
	context_info.uid = -1;
	context_info.options = 0;
	context_info.user = NULL;
	context_info.ka_time = 0;
	context_info.ka_probes = 0;
	context_info.ka_interval = 0;
	
	const char *cert_path = NULL;
	const char *key_path = NULL;
	
	int opts = 0;
	
//	context = libwebsocket_create_context( port, interface, protocols,
//														libwebsocket_internal_extensions,
//														cert_path, key_path, -1, -1, opts );
	
	context = libwebsocket_create_context( &context_info );
	
	if ( context == NULL )
	{
		fprintf( stderr, "libwebsocket init failed\n" );
		return -1;
	}
	
	printf( "Starting server ...\n" );
	
	while ( 1 )
	{
		libwebsocket_service( context, 50 );
	}
	
	libwebsocket_context_destroy( context );
	
	return 0;
}

