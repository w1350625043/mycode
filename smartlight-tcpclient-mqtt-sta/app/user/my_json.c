/*
 * my_json.c
 *
 *  Created on: 2016��9��3��
 *      Author: Administrator
 */


#include "user_json.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"

LOCAL int jsonTree_set(struct jsontree_context *js_ctx, struct jsonparse_state *parser);
LOCAL int jsonArray_set(struct jsontree_context *js_ctx, struct jsonparse_state *parser);
void setJsonObject(char *json);
LOCAL int jsonObject_set(struct jsonparse_state *parser);

//LOCAL int ICACHE_FLASH_ATTR
//jsonTree_get(struct jsontree_context *js_ctx)
//{
//    const char *path = jsontree_path_name(js_ctx, js_ctx->depth - 1);
//
//    if (os_strncmp(path, "String", os_strlen("String")) == 0) {
//    	jsontree_write_string(js_ctx, "data");
//    } else if (os_strncmp(path, "Integer", os_strlen("Integer")) == 0) {
//        jsontree_write_int(js_ctx, 1);
//    } else if (os_strncmp(path, "Array", os_strlen("Array")) == 0) {
//    	int array[3] = {0,1,2};
//    	jsontree_write_atom(js_ctx, "[");
//    	jsontree_write_int_array(js_ctx, array, 3);
//    	jsontree_write_atom(js_ctx, "]");
//    }
//
//    return 0;
//}
//
//LOCAL int ICACHE_FLASH_ATTR
//jsonArray_get(struct jsontree_context *js_ctx)
//{
//    const char *path = jsontree_path_name(js_ctx, js_ctx->depth - 1);
//
//    if (os_strncmp(path, "K1", os_strlen("K2")) == 0) {
//    	jsontree_write_string(js_ctx, "D1");
//    } else if (os_strncmp(path, "K2", os_strlen("K2")) == 0) {
//    	jsontree_write_string(js_ctx, "D2");
//    } else if (os_strncmp(path, "K3", os_strlen("K3")) == 0) {
//    	jsontree_write_string(js_ctx, "D3");
//    }
//
//    return 0;
//}
//
//
//
//
//
//
//
//LOCAL struct jsontree_callback jsonArrayCallback =
//    JSONTREE_CALLBACK(jsonArray_get, jsonArray_set);
//
//JSONTREE_OBJECT(jsonArrayData,
//                JSONTREE_PAIR("K1", &jsonArrayCallback),
//                JSONTREE_PAIR("K2", &jsonArrayCallback),
//				JSONTREE_PAIR("K3", &jsonArrayCallback));
//JSONTREE_ARRAY(jsonArray,
//               JSONTREE_PAIR_ARRAY(&jsonArrayData),
//               JSONTREE_PAIR_ARRAY(&jsonArrayData),
//               JSONTREE_PAIR_ARRAY(&jsonArrayData));
//
//LOCAL struct jsontree_callback jsonCallback =
//    JSONTREE_CALLBACK(jsonTree_get, jsonTree_set);
//
//JSONTREE_OBJECT(jsonObject,
//                JSONTREE_PAIR("String", &jsonCallback),
//                JSONTREE_PAIR("Integer", &jsonCallback),
//                JSONTREE_PAIR("JsonArray", &jsonArray));
//JSONTREE_OBJECT(jsonTestTrees,
//                JSONTREE_PAIR("String", &jsonCallback),
//				JSONTREE_PAIR("Integer", &jsonCallback),
//				JSONTREE_PAIR("Array", &jsonCallback),
//				JSONTREE_PAIR("JsonObject", &jsonObject));
//JSONTREE_OBJECT(jsonTestTree,
//                JSONTREE_PAIR("jsonTest", &jsonTestTrees));
//
//
//#define LENGTH 512
//char* ICACHE_FLASH_ATTR
//getJsonTree(void)
//{
//    static char jsonbuf[LENGTH];
//    os_memset(jsonbuf, 0, LENGTH);      //��ʼ���ַ���
//	json_ws_send((struct jsontree_value *)&jsonTestTree, "jsonTest", jsonbuf);
//
//	//os_printf("%s\n", jsonbuf);
//
//	return jsonbuf;
//}



LOCAL int ICACHE_FLASH_ATTR
jsonTree_set(struct jsontree_context *js_ctx, struct jsonparse_state *parser)
{
    int type;

    while ((type = jsonparse_next(parser)) != 0) {
    	//�����KEY����
        if (type == JSON_TYPE_PAIR_NAME) {
            char buffer[64];
            os_bzero(buffer, 64);

            if (jsonparse_strcmp_value(parser, "String") == 0) {
                jsonparse_next(parser);	//���ص���ð���ַ�
                type = jsonparse_next(parser);	//���ص���˫�����ַ�
                os_printf("String Value type = %c\n", type);	// = "

                //���Value���ַ������ͣ����ȡ���ݵ�buffer
                if (JSON_TYPE_STRING == type){	//#define JSON_TYPE_STRING '"'
                	jsonparse_copy_value(parser, buffer, sizeof(buffer));
                	os_printf("String Value = %s\n", buffer);
                }

            } else if (jsonparse_strcmp_value(parser, "Integer") == 0) {
                jsonparse_next(parser);
                type = jsonparse_next(parser);


                os_printf("Integer Value type = %c\n", type);	// = 0
                //���Value����ֵ����
                if(JSON_TYPE_NUMBER == type){	//#define JSON_TYPE_NUMBER '0'
                	//jsonparse_copy_value(parser, buffer, sizeof(buffer));
                	int num = 0;
                	num = jsonparse_get_value_as_int(parser);
                	os_printf("Integer Value = %d\n", num);		// = 1
                }
            } else if (jsonparse_strcmp_value(parser, "Array") == 0) {
                jsonparse_next(parser);	//����ð��
                type = jsonparse_next(parser);
                os_printf("Array Value type = %c\n", type);		// = [

                //���Value����������
                if(JSON_TYPE_ARRAY == type){	//#define JSON_TYPE_ARRAY '['
                	//jsonparse_copy_value(parser, buffer, sizeof(buffer));
                	int length = jsonparse_get_len(parser);
                	os_printf("Array Length = %d\n", length);	//= 5, ������[0,1,2]�����ܰѶ���Ҳ������

                	int i;
                	int num = 100;
                	//ѭ����ȡ���������
                	for(i=0; i<length; i++){
                		type = jsonparse_next(parser);
                		// ����Ƕ��ţ���ֱ�Ӵ�ӡ, �������ֵ���ӡ0
                		os_printf("Array[%d] type = %c ", i, type);

                		//�������ֵ���ͣ���ת����int����ӡ����
                		if(JSON_TYPE_NUMBER==type){
                        	num = jsonparse_get_value_as_int(parser);
                        	os_printf("Array[%d] = %d\n", i, num);
                		}
                		//����������else if�ж��Ƿ�����������
                		//���� else if(JSON_TYPE_OBJECT==type),�ж��Ƿ���Json����
                		else{
                			os_printf("\n");
                		}
                	}
                }
            }
#if 1
            else if (jsonparse_strcmp_value(parser, "JsonObject") == 0) {
                jsonparse_next(parser);	//����ð��
                type = jsonparse_next(parser);
                os_printf("JsonObject Value type = %c\n", type);		// = {

                if(JSON_TYPE_OBJECT == type){	//#define JSON_TYPE_OBJECT '{'
                	int length = jsonparse_get_len(parser);
                	os_printf("JsonObject Length = %d\n", length);

                	//char temp[128] = {0};
                	//jsonparse_copy_value(parser, temp, 128);
                	//os_printf("JsonObject Value = %s\n", temp);

                	//ѭ����ȡ����
                	int i = 0;
                	//for(i=0; i<length; i++){
                	while(type != '}'){
                		i++;
                		type = jsonparse_next(parser);

                		os_printf("JsonObject[%d] type = %c", i, type);
                		os_printf("\n");
                		//os_memset(temp, 0, 128);
                    	//jsonparse_copy_value(parser, temp, 128);
                    	//os_printf("JsonObject Value = %s\n", temp);
                		//��ȡ�ڶ����Json���������
                		jsonObject_set(parser);
                	}
                }

            }
#endif
        }
    }

    return 0;
}

LOCAL int ICACHE_FLASH_ATTR
jsonObject_set(struct jsonparse_state *parser)
{
	int type = jsonparse_get_type(parser);
	int vtype = parser->vtype;
	//os_printf("json Object type=%c, vtype=%c\n", type, vtype);
	char buffer[64];
	os_bzero(buffer, 64);

	//�����KEY����
	if (vtype == JSON_TYPE_PAIR_NAME) {
	   if (jsonparse_strcmp_value(parser, "String") == 0) {
			jsonparse_next(parser);	//���ص���ð���ַ�
			type = jsonparse_next(parser);	//���ص���˫�����ַ�
			//os_printf("jsonObject String Value type = %c\n", type);	// = "

			//���Value���ַ������ͣ����ȡ���ݵ�buffer
			if (JSON_TYPE_STRING == type){	//#define JSON_TYPE_STRING '"'
				jsonparse_copy_value(parser, buffer, sizeof(buffer));
				os_printf("jsonObject String Value = %s\n", buffer);
			}

		} else if (jsonparse_strcmp_value(parser, "Integer") == 0) {
			jsonparse_next(parser);
			type = jsonparse_next(parser);
			//os_printf("jsonObject Integer Value type = %c\n", type);	// = 0
			//���Value����ֵ����
			if(JSON_TYPE_NUMBER == type){	//#define JSON_TYPE_NUMBER '0'
				//jsonparse_copy_value(parser, buffer, sizeof(buffer));
				int num = 0;
				num = jsonparse_get_value_as_int(parser);
				os_printf("jsonObject Integer Value = %d\n", num);		// = 1
			}
		} else if (jsonparse_strcmp_value(parser, "JsonArray") == 0) {
			jsonparse_next(parser);
			type = jsonparse_next(parser);
			//os_printf("jsonObject Integer Value type = %c\n", type);	// = 0
			//���Value����ֵ����
			if(JSON_TYPE_ARRAY == type){
				//jsonparse_copy_value(parser, buffer, sizeof(buffer));
				//os_printf("buffer = %s\n", buffer);
            	int length = jsonparse_get_len(parser);
            	int vlen = parser->vlen;
            	os_printf("JsonArray Length = %d vlen = %d\n", length, vlen);	//��ȡ�����ĳ��Ȳ�׼ȷ

            	//ѭ����ȡJson��������
				int i = 0;
				//for(i=0; i<length; i++){
				while(type != ']'){i++;		//��']'�ж��Ƿ�ﵽ����ĩβ

					type = jsonparse_next(parser);

					os_printf("JsonArray[%d] type = %c", i, type);
					os_printf("\n");

					//�����KEY����
					if (type == JSON_TYPE_PAIR_NAME) {
					   if (jsonparse_strcmp_value(parser, "K1") == 0) {
							jsonparse_next(parser);	//���ص���ð���ַ�
							type = jsonparse_next(parser);	//���ص���˫�����ַ�
							//os_printf("K1 Value type = %c\n", type);	// = "

							//���Value���ַ������ͣ����ȡ���ݵ�buffer
							if (JSON_TYPE_STRING == type){	//#define JSON_TYPE_STRING '"'
								os_bzero(buffer, 64);
								jsonparse_copy_value(parser, buffer, sizeof(buffer));
								os_printf("K1 = %s\n", buffer);
							}
						} else if(jsonparse_strcmp_value(parser, "K2") == 0){
							jsonparse_next(parser);	//���ص���ð���ַ�
							type = jsonparse_next(parser);	//���ص���˫�����ַ�
							//���Value���ַ������ͣ����ȡ���ݵ�buffer
							if (JSON_TYPE_STRING == type){	//#define JSON_TYPE_STRING '"'
								os_bzero(buffer, 64);
								jsonparse_copy_value(parser, buffer, sizeof(buffer));
								os_printf("K2 = %s\n", buffer);
							}
						} else if(jsonparse_strcmp_value(parser, "K3") == 0){
							jsonparse_next(parser);	//���ص���ð���ַ�
							type = jsonparse_next(parser);	//���ص���˫�����ַ�
							//���Value���ַ������ͣ����ȡ���ݵ�buffer
							if (JSON_TYPE_STRING == type){	//#define JSON_TYPE_STRING '"'
								os_bzero(buffer, 64);
								jsonparse_copy_value(parser, buffer, sizeof(buffer));
								os_printf("K3 = %s\n", buffer);
							}
						}
					}
				}
			}
		}
	}
}



LOCAL int ICACHE_FLASH_ATTR
jsonArray_set(struct jsontree_context *js_ctx, struct jsonparse_state *parser)
{
	int type;

	while ((type = jsonparse_next(parser)) == 0) {
		//�����KEY����
		if (type == JSON_TYPE_PAIR_NAME) {
			char buffer[64];
			os_bzero(buffer, 64);

			if (jsonparse_strcmp_value(parser, "K1") == 0) {
                if (JSON_TYPE_STRING == type){	//#define JSON_TYPE_STRING '"'
                	jsonparse_copy_value(parser, buffer, sizeof(buffer));
                	os_printf("K1 Value = %s\n", buffer);
                }
			} else if (jsonparse_strcmp_value(parser, "K2")==0) {
                if (JSON_TYPE_STRING == type){	//#define JSON_TYPE_STRING '"'
                	jsonparse_copy_value(parser, buffer, sizeof(buffer));
                	os_printf("K2 Value = %s\n", buffer);
                }
			} else if (jsonparse_strcmp_value(parser, "K3")==0) {
                if (JSON_TYPE_STRING == type){	//#define JSON_TYPE_STRING '"'
                	jsonparse_copy_value(parser, buffer, sizeof(buffer));
                	os_printf("K3 Value = %s\n", buffer);
                }
			}
		}
	}

    return 0;
}




//void ICACHE_FLASH_ATTR
//setJsonTree(char *json)
//{
//    struct jsontree_context js;
//
//    jsontree_setup(&js, (struct jsontree_value *)&jsonTestTree, json_putchar);
//    json_parse(&js, json);
//}

//void ICACHE_FLASH_ATTR
//setJsonObject(char *json)
//{
//    struct jsontree_context js;
//
//    jsontree_setup(&js, (struct jsontree_value *)&jsonObject, json_putchar);
//    json_parse(&js, json);
//}
//



