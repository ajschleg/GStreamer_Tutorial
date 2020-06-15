#include <gst/gst.h>

/* To make this application on Linux run gcc basic-tutorial-2.c -o basic-tutorial-2 `pkg-config --cflags --libs gstreamer-1.0`*/

int main(int argc, char *argv[]) {
  GstElement *pipeline, *source, *sink, *filter, *converter;
  GstBus *bus;
  GstMessage *msg;
  GstStateChangeReturn ret;

  /* Initialize GStreamer */
  gst_init (&argc, &argv);

  /* Create the elements */
	/*videotestsrc is a source element, which creates a test video pattern. Useful for debugging not usually found in real applications. */
  source = gst_element_factory_make ("videotestsrc", "source");
	/* autovideosink is a sink element, which displays on a window the images it receives. There are several video sinks, depends on the OS. autovideosink automatically selects and instantiates the best one and is platform independant. */
  sink = gst_element_factory_make ("autovideosink", "sink");
	
	/*vertigotv is a filter with a nice effect. */
  filter = gst_element_factory_make ( "vertigotv", "filter");

	/*videoconvert to get rid of negotiation error in between filter and sink. */
  converter = gst_element_factory_make( "videoconvert", "converter");

  /* Create the empty pipeline */
	/* All elements in GStreamer typically must be contained in a pipeline.*/
  pipeline = gst_pipeline_new ("test-pipeline");

  if (!pipeline || !source || !sink || !filter || !converter) {
    g_printerr ("Not all elements could be created.\n");
    return -1;
  }

  /* Build the pipeline */
	/* A pipeline is a particular type of bin, which the element used to contain the other elements. */
  gst_bin_add_many (GST_BIN (pipeline), source, filter, converter, sink, NULL);
	/* Link the source to the filter. Elements must be in the same bin in order to be linked. */
  if (gst_element_link_many (source, filter, converter, sink, NULL) != TRUE) {
    g_printerr ("Elements could not be linked.\n");
    gst_object_unref (pipeline);
    return -1;
  }

  /* Modify the source's properties */
  g_object_set (source, "pattern", 1, NULL);

  /* Start playing */
  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    gst_object_unref (pipeline);
    return -1;
  }

  /* Wait until error or EOS */
  bus = gst_element_get_bus (pipeline);
  msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

  /* Parse message */
  if (msg != NULL) {
    GError *err;
    gchar *debug_info;

    switch (GST_MESSAGE_TYPE (msg)) {
      case GST_MESSAGE_ERROR:
        gst_message_parse_error (msg, &err, &debug_info);
        g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
        g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
        g_clear_error (&err);
        g_free (debug_info);
        break;
      case GST_MESSAGE_EOS:
        g_print ("End-Of-Stream reached.\n");
        break;
      default:
        /* We should not reach here because we only asked for ERRORs and EOS */
        g_printerr ("Unexpected message received.\n");
        break;
    }
    gst_message_unref (msg);
  }

  /* Free resources */
  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  return 0;
}
